#include "internal.h"

static const char k_wordpack[] =
    " etaoinshrdlcumwfgypbvkjxqz0123456789 !?.,:;"
    "()-\x26*\x5c'@#+=\xA3$%\"[]";

rs_err rs_wordpack_decode(rs_buf *src, int nbytes, char *dst, size_t dst_cap) {
    int i, n = 0, hold = -1;
    if (!src || !dst || dst_cap == 0) {
        return RS_ERR_ARG;
    }
    dst[0] = 0;
    for (i = 0; i < nbytes && rs_buf_remaining(src) > 0; i++) {
        int b = rs_g1(src);
        int hi = (b >> 4) & 15, lo = b & 15, k;
        for (k = 0; k < 2; k++) {
            int nib = k ? lo : hi;
            if (hold == -1) {
                if (nib < 13) {
                    if ((size_t)n + 1 < dst_cap) {
                        dst[n++] = k_wordpack[nib];
                    }
                } else {
                    hold = nib;
                }
            } else {
                int idx = (hold << 4) + nib - 195;
                if (idx >= 0 && idx < (int)sizeof(k_wordpack) - 1 && (size_t)n + 1 < dst_cap) {
                    dst[n++] = k_wordpack[idx];
                }
                hold = -1;
            }
        }
    }
    dst[n] = 0;
    {
        int cap = 1, j;
        for (j = 0; dst[j]; j++) {
            if (cap && dst[j] >= 'a' && dst[j] <= 'z') {
                dst[j] = (char)(dst[j] - 32);
                cap = 0;
            }
            if (dst[j] == '.' || dst[j] == '!' || dst[j] == '?') {
                cap = 1;
            }
        }
    }
    return RS_OK;
}

rs_err rs_wordpack_encode(const char *text, rs_buf *dst) {
    char tmp[81];
    int i, n, hold = -1;
    if (!text || !dst) {
        return RS_ERR_ARG;
    }
    n = (int)strlen(text);
    if (n > 80) {
        n = 80;
    }
    for (i = 0; i < n; i++) {
        char c = text[i];
        if (c >= 'A' && c <= 'Z') {
            c = (char)(c + 32);
        }
        tmp[i] = c;
    }
    tmp[n] = 0;
    for (i = 0; i < n; i++) {
        int idx = 0, j, v;
        for (j = 0; k_wordpack[j]; j++) {
            if (tmp[i] == k_wordpack[j]) {
                idx = j;
                break;
            }
        }
        v = idx > 12 ? idx + 195 : idx;
        if (hold == -1) {
            if (v < 13) {
                hold = v;
            } else {
                rs_err e = rs_p1(dst, v);
                if (e) {
                    return e;
                }
            }
        } else if (v < 13) {
            rs_err e = rs_p1(dst, (hold << 4) + v);
            if (e) {
                return e;
            }
            hold = -1;
        } else {
            rs_err e = rs_p1(dst, (hold << 4) + (v >> 4));
            if (e) {
                return e;
            }
            hold = v & 15;
        }
    }
    if (hold != -1) {
        return rs_p1(dst, hold << 4);
    }
    return RS_OK;
}

rs_err rs_huffman_init(rs_huffman *h, const uint8_t *bits, size_t nsym) {
    uint32_t next[33];
    int symbol, next_node = 0;
    if (!h || !bits || nsym == 0 || nsym > RS_HUFFMAN_SYMS) {
        return RS_ERR_ARG;
    }
    memset(h, 0, sizeof(*h));
    memcpy(h->bits, bits, nsym);
    h->tree_len = 8;
    memset(next, 0, sizeof(next));
    for (symbol = 0; symbol < (int)nsym; symbol++) {
        int nbits = h->bits[symbol], bit, code, node, b;
        if (nbits == 0) {
            continue;
        }
        bit = 1 << (32 - nbits);
        code = (int)next[nbits];
        h->codes[symbol] = (uint32_t)code;
        if ((code & bit) == 0) {
            int k;
            for (k = nbits - 1; k >= 1; k--) {
                int cur = (int)next[k];
                if (code != cur) {
                    break;
                }
                if (cur & (1 << (32 - k))) {
                    next[k] = next[k - 1];
                    break;
                }
                next[k] = (uint32_t)(cur | (1 << (32 - k)));
            }
            next[nbits] = (uint32_t)(code | bit);
        } else {
            next[nbits] = next[nbits - 1];
        }
        {
            int k;
            uint32_t filled = next[nbits];
            for (k = nbits + 1; k <= 32; k++) {
                if (next[k] == (uint32_t)code) {
                    next[k] = filled;
                }
            }
        }
        node = 0;
        for (b = 0; b < nbits; b++) {
            int mask = (int)((unsigned)1 << 31) >> b;
            if ((code & mask) == 0) {
                node++;
            } else {
                if (node >= h->tree_len) {
                    return RS_ERR_RANGE;
                }
                if (h->tree[node] == 0) {
                    if (h->tree_len + 2 >= (int)(sizeof(h->tree) / sizeof(h->tree[0]))) {
                        return RS_ERR_RANGE;
                    }
                    h->tree[node] = h->tree_len;
                    h->tree_len += 2;
                }
                node = h->tree[node];
            }
            if (node >= (int)(sizeof(h->tree) / sizeof(h->tree[0]))) {
                return RS_ERR_RANGE;
            }
        }
        h->tree[node] = ~symbol;
        (void)next_node;
    }
    h->ready = 1;
    return RS_OK;
}

rs_err rs_huffman_encode(const rs_huffman *h, const char *text, rs_buf *dst) {
    uint32_t acc = 0;
    int bits = 0;
    size_t i, n;
    if (!h || !h->ready || !text || !dst) {
        return RS_ERR_ARG;
    }
    n = strlen(text);
    for (i = 0; i <= n; i++) {
        int ch = i == n ? 0 : (text[i] & 0xff);
        int nbits = h->bits[ch];
        uint32_t code = h->codes[ch];
        acc |= code >> bits;
        bits += nbits;
        while (bits >= 8) {
            rs_err e = rs_p1(dst, (int)(acc >> 24));
            if (e) {
                return e;
            }
            acc <<= 8;
            bits -= 8;
        }
    }
    if (bits > 0) {
        return rs_p1(dst, (int)(acc >> 24));
    }
    return RS_OK;
}

rs_err rs_huffman_decode(const rs_huffman *h, rs_buf *src, int nbytes, char *dst, size_t dst_cap) {
    int node = 0, n = 0, i;
    if (!h || !h->ready || !src || !dst || dst_cap == 0) {
        return RS_ERR_ARG;
    }
    dst[0] = 0;
    for (i = 0; i < nbytes && rs_buf_remaining(src) > 0; i++) {
        int b = rs_g1(src), bit;
        for (bit = 0; bit < 8; bit++) {
            node = ((b & 0x80) == 0) ? node + 1 : h->tree[node];
            b = (b << 1) & 0xff;
            if (node < 0 || node >= h->tree_len) {
                return RS_ERR_FORMAT;
            }
            if (h->tree[node] < 0) {
                int sym = ~h->tree[node];
                if (sym == 0) {
                    dst[n] = 0;
                    return RS_OK;
                }
                if ((size_t)n + 1 < dst_cap) {
                    dst[n++] = (char)sym;
                }
                node = 0;
            }
        }
    }
    dst[n] = 0;
    return RS_OK;
}

rs_err rs_chat_encode(const rs_build_info *build, const rs_huffman *h, const char *text,
                      rs_buf *dst) {
    if (build && build->era == RS_ERA_JS5 && h && h->ready) {
        return rs_huffman_encode(h, text, dst);
    }
    return rs_wordpack_encode(text, dst);
}

rs_err rs_chat_decode(const rs_build_info *build, const rs_huffman *h, rs_buf *src, int nbytes,
                      char *dst, size_t dst_cap) {
    if (build && build->era == RS_ERA_JS5 && h && h->ready) {
        return rs_huffman_decode(h, src, nbytes, dst, dst_cap);
    }
    return rs_wordpack_decode(src, nbytes, dst, dst_cap);
}
