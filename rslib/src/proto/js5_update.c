#include "proto/ops.h"

rs_err rs_js5_write_status(rs_buf *dst, int status) {
    return rs_p1(dst, status);
}

/* 910 ClientJs5TcpClient writes every JS5 request as 6 bytes. Earlier
 * clients use 4 bytes before revision 500 and a shorter control packet. */
static size_t js5_request_len(int rev, int opcode) {
    if (rev >= 790) {
        return 6;
    }
    if (opcode == RS_JS5_PREFETCH || opcode == RS_JS5_URGENT) {
        return (rev > 0 && rev < 500) ? 4 : 6;
    }
    return (rev >= 508) ? 5 : 4;
}

rs_err rs_js5_parse_request(const rs_build_info *build, const uint8_t *data, size_t len,
                            rs_js5_req *out, size_t *consumed) {
    int rev = build ? build->revision : 0;
    if (!data || !out) {
        return RS_ERR_ARG;
    }
    if (len < 1) {
        return RS_ERR_NEED_MORE;
    }
    out->opcode = data[0] & 0xff;
    if (out->opcode >= 2 && out->opcode != RS_JS5_PREFETCH && out->opcode != RS_JS5_URGENT) {
        size_t need = js5_request_len(rev, out->opcode);
        if (len < need) {
            return RS_ERR_NEED_MORE;
        }
        out->archive = 0;
        out->group = 0;
        if (out->opcode == RS_JS5_ENCRYPT) {
            out->archive = (rev >= 790) ? (data[1] & 0xff) : (data[need - 1] & 0xff);
        }
        if (consumed) {
            *consumed = need;
        }
        return RS_OK;
    }
    if (rev > 0 && rev < 500) {
        if (len < 4) {
            return RS_ERR_NEED_MORE;
        }
        out->archive = data[1] & 0xff;
        out->group = (data[2] << 8) | (data[3] & 0xff);
        if (consumed) {
            *consumed = 4;
        }
        return RS_OK;
    }
    if (len < 6) {
        return RS_ERR_NEED_MORE;
    }
    out->archive = data[1] & 0xff;
    out->group = (int)(((uint32_t)data[2] << 24) | ((uint32_t)data[3] << 16) |
                       ((uint32_t)data[4] << 8) | (uint32_t)data[5]);
    if (consumed) {
        *consumed = 6;
    }
    return RS_OK;
}

rs_err js5_emit_framed(rs_buf *dst, const uint8_t *hdr, size_t hdr_len, const uint8_t *payload,
                              size_t payload_len) {
    size_t i, block = 0;
    rs_err e;
    for (i = 0; i < hdr_len; i++) {
        if (block == 512) {
            e = rs_p1(dst, 0xff);
            if (e) {
                return e;
            }
            block = 0;
        }
        e = rs_p1(dst, hdr[i]);
        if (e) {
            return e;
        }
        block++;
    }
    for (i = 0; i < payload_len; i++) {
        if (block == 512) {
            e = rs_p1(dst, 0xff);
            if (e) {
                return e;
            }
            block = 0;
        }
        e = rs_p1(dst, payload[i]);
        if (e) {
            return e;
        }
        block++;
    }
    return RS_OK;
}

rs_err rs_js5_write_group(const rs_build_info *build, rs_buf *dst, int archive, int group,
                          const uint8_t *container, size_t len, int urgent) {
    uint8_t hdr[16];
    size_t hlen;
    int compression, clen;
    const uint8_t *payload;
    size_t payload_len;
    int rev = build ? build->revision : 0;

    if (!container || len < 5) {
        return RS_ERR_FORMAT;
    }
    compression = container[0];
    clen = (int)rs_be32u(container + 1);
    (void)urgent;
    if (rev >= 508) {
        hdr[0] = (uint8_t)archive;
        rs_put_be32(hdr + 1, (uint32_t)group);
        hdr[5] = (uint8_t)compression;
        rs_put_be32(hdr + 6, (uint32_t)clen);
        hlen = 10;
        payload = container + 5;
        payload_len = len - 5;
    } else {
        hdr[0] = (uint8_t)archive;
        rs_put_be16(hdr + 1, group);
        hdr[3] = (uint8_t)compression;
        rs_put_be32(hdr + 4, (uint32_t)clen);
        hlen = 8;
        payload = container + 5;
        payload_len = len - 5;
    }
    return js5_emit_framed(dst, hdr, hlen, payload, payload_len);
}

rs_err js5_update_parse(rs_session *s, const uint8_t *data, size_t len, rs_update_req *out,
                               size_t *consumed) {
    rs_js5_req js5;
    rs_err e;
    if (!out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = rs_js5_parse_request(s ? s->build : NULL, data, len, &js5, consumed);
    if (e != RS_OK) {
        return e;
    }
    out->opcode = js5.opcode;
    out->archive = js5.archive;
    out->id = js5.group;
    out->priority = (js5.opcode == RS_JS5_URGENT) ? 2 : 0;
    if (s && js5.opcode == RS_JS5_ENCRYPT && consumed && *consumed >= 2) {
        int rev = s->build ? s->build->revision : 0;
        s->js5_encrypt = 1;
        s->js5_key = (uint8_t)((rev >= 790) ? data[1] : data[*consumed - 1]);
    }
    return RS_OK;
}

rs_err js5_update_write(rs_session *s, rs_buf *dst, const rs_update_req *req) {
    const rs_build_info *b = s ? s->build : NULL;
    int rev = b ? b->revision : 0;
    rs_err e;
    if (!req) {
        return RS_ERR_ARG;
    }
    if (req->opcode >= 2 && req->opcode != RS_JS5_PREFETCH && req->opcode != RS_JS5_URGENT) {
        size_t need = js5_request_len(rev, req->opcode);
        uint8_t raw[8];
        memset(raw, 0, sizeof(raw));
        raw[0] = (uint8_t)req->opcode;
        if (req->opcode == RS_JS5_ENCRYPT) {
            int key = req->key ? req->key : req->archive;
            raw[(rev >= 790) ? 1 : (int)need - 1] = (uint8_t)key;
            if (s) {
                s->js5_encrypt = 1;
                s->js5_key = (uint8_t)key;
            }
        } else if (rev >= 790 && req->opcode == RS_JS5_CONNECTED) {
            raw[3] = 4; /* p3(4), then p2(0) */
        }
        return rs_pdata(dst, raw, need);
    }
    e = rs_p1(dst, req->opcode);
    if (e) {
        return e;
    }
    e = rs_p1(dst, req->archive);
    if (e) {
        return e;
    }
    if (rev > 0 && rev < 500) {
        return rs_p2(dst, req->id);
    }
    return rs_p4(dst, req->id);
}

rs_err js5_update_reply(rs_session *s, rs_buf *dst, const rs_update_req *req,
                               const uint8_t *data, size_t len) {
    size_t start;
    rs_err e;
    if (!s || !dst || !req) {
        return RS_ERR_ARG;
    }
    if (!data || !len) {
        return RS_OK;
    }
    start = dst->pos;
    e = rs_js5_write_group(s->build, dst, req->archive, req->id, data, len, req->opcode == 1);
    if (e == RS_OK && s->js5_encrypt) {
        size_t i;
        for (i = start; i < dst->pos; i++) {
            dst->data[i] = (uint8_t)(dst->data[i] ^ s->js5_key);
        }
    }
    return e;
}

static rs_err js5_reply_append(rs_js5_reply *st, uint8_t b) {
    if (st->len + 1 > st->cap) {
        size_t ncap = st->cap ? st->cap * 2 : 256;
        uint8_t *p;
        if (ncap < st->len + 1) {
            ncap = st->len + 1;
        }
        p = rs_realloc(st->buf, ncap);
        if (!p) {
            return RS_ERR_NOMEM;
        }
        st->buf = p;
        st->cap = ncap;
    }
    st->buf[st->len++] = b;
    return RS_OK;
}

static rs_err js5_reply_header(rs_js5_reply *st, int hlen) {
    unsigned comp;
    uint32_t clen;
    uint64_t payload;
    if (hlen >= 10) {
        comp = st->buf[5];
        clen = rs_be32u(st->buf + 6);
    } else {
        comp = st->buf[3];
        clen = rs_be32u(st->buf + 4);
    }
    payload = clen;
    if ((comp & 0x7fu) != 0) {
        payload += 4;
    }
    if (payload > (8u * 1024u * 1024u)) {
        return RS_ERR_FORMAT;
    }
    st->need = (size_t)hlen + (size_t)payload;
    st->have_hdr = hlen;
    return RS_OK;
}

static rs_err js5_reply_emit(rs_js5_reply *st, int hlen, rs_js5_reply_fn fn, void *user) {
    unsigned comp;
    uint32_t clen;
    int archive, group;
    size_t payload;
    uint8_t *box;
    if (hlen >= 10) {
        archive = st->buf[0] & 0xff;
        group = (int)rs_be32u(st->buf + 1);
        comp = st->buf[5];
        clen = rs_be32u(st->buf + 6);
    } else {
        archive = st->buf[0] & 0xff;
        group = rs_be16(st->buf + 1);
        comp = st->buf[3];
        clen = rs_be32u(st->buf + 4);
    }
    payload = st->need - (size_t)hlen;
    box = rs_malloc(5 + payload);
    if (!box) {
        return RS_ERR_NOMEM;
    }
    box[0] = (uint8_t)(comp & 0x7fu);
    rs_put_be32(box + 1, clen);
    if (payload) {
        memcpy(box + 5, st->buf + hlen, payload);
    }
    if (fn) {
        fn(user, archive, group, box, 5 + payload);
    }
    free(box);
    return RS_OK;
}

rs_err rs_js5_reply_feed(rs_js5_reply *st, int revision, const uint8_t *data, size_t len,
                         rs_js5_reply_fn fn, void *user) {
    size_t i;
    int hlen;
    if (!st || (len > 0 && !data)) {
        return RS_ERR_ARG;
    }
    hlen = revision >= 508 ? 10 : 8;
    for (i = 0; i < len; i++) {
        uint8_t b = data[i];
        rs_err e;
        if (st->encrypt) {
            b = (uint8_t)(b ^ st->key);
        }
        if (st->block == 512) {
            if (b != 0xff) {
                return RS_ERR_PROTO;
            }
            st->block = 0;
            continue;
        }
        e = js5_reply_append(st, b);
        if (e != RS_OK) {
            return e;
        }
        st->block++;
        if (!st->have_hdr && st->len >= (size_t)hlen) {
            e = js5_reply_header(st, hlen);
            if (e != RS_OK) {
                return e;
            }
        }
        if (st->have_hdr && st->len >= st->need) {
            e = js5_reply_emit(st, st->have_hdr, fn, user);
            if (e != RS_OK) {
                return e;
            }
            st->len = 0;
            st->have_hdr = 0;
            st->need = 0;
            st->block = 0;
        }
    }
    return RS_OK;
}

void rs_js5_reply_free(rs_js5_reply *st) {
    if (!st) {
        return;
    }
    free(st->buf);
    memset(st, 0, sizeof(*st));
}
