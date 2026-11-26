#include "both/buf_impl.h"

int rs_g1(rs_buf *b) {
    if (!rs_buf_has(b, 1)) {
        return 0;
    }
    return b->data[b->pos++] & 0xff;
}

int rs_g1b(rs_buf *b) {
    return (int8_t)rs_g1(b);
}

int rs_g2(rs_buf *b) {
    if (!rs_buf_has(b, 2)) {
        return 0;
    }
    b->pos += 2;
    return rs_be16(b->data + b->pos - 2);
}

int rs_g2b(rs_buf *b) {
    int v = rs_g2(b);
    if (v > 32767) {
        v -= 65536;
    }
    return v;
}

int rs_g3(rs_buf *b) {
    if (!rs_buf_has(b, 3)) {
        return 0;
    }
    b->pos += 3;
    return rs_be24(b->data + b->pos - 3);
}

int rs_g4(rs_buf *b) {
    if (!rs_buf_has(b, 4)) {
        return 0;
    }
    b->pos += 4;
    return (int)rs_be32u(b->data + b->pos - 4);
}

int32_t rs_g4s(rs_buf *b) {
    return (int32_t)rs_g4(b);
}

uint64_t rs_g8(rs_buf *b) {
    uint32_t hi = (uint32_t)rs_g4(b);
    uint32_t lo = (uint32_t)rs_g4(b);
    return ((uint64_t)hi << 32) | lo;
}

int rs_gsmart(rs_buf *b) {
    if (!rs_buf_has(b, 1)) {
        return 0;
    }
    if ((b->data[b->pos] & 0xff) < 128) {
        return rs_g1(b) - 64;
    }
    return rs_g2(b) - 49152;
}

int rs_gsmarts(rs_buf *b) {
    if (!rs_buf_has(b, 1)) {
        return 0;
    }
    if ((b->data[b->pos] & 0xff) < 128) {
        return rs_g1(b);
    }
    return rs_g2(b) - 32768;
}

int rs_gsmart2or4(rs_buf *b) {
    if (!rs_buf_has(b, 1)) {
        return 0;
    }
    if ((int8_t)b->data[b->pos] < 0) {
        return rs_g4s(b) & 0x7fffffff;
    }
    return rs_g2(b);
}

rs_err rs_gdata(rs_buf *b, void *dst, size_t n) {
    if (!rs_buf_has(b, n)) {
        return RS_ERR_RANGE;
    }
    memcpy(dst, b->data + b->pos, n);
    b->pos += n;
    return RS_OK;
}

rs_err rs_gjstr(rs_buf *b, char *dst, size_t dst_cap) {
    size_t start = b->pos;
    size_t n;
    while (b->pos < b->cap && b->data[b->pos] != 10) {
        b->pos++;
    }
    if (b->pos >= b->cap) {
        return RS_ERR_FORMAT;
    }
    n = b->pos - start;
    if (n + 1 > dst_cap) {
        return RS_ERR_RANGE;
    }
    memcpy(dst, b->data + start, n);
    dst[n] = 0;
    b->pos++; /* skip 10 */
    return RS_OK;
}

rs_err rs_gjstrnull(rs_buf *b, char *dst, size_t dst_cap) {
    size_t start = b->pos;
    size_t n;
    while (b->pos < b->cap && b->data[b->pos] != 0) {
        b->pos++;
    }
    if (b->pos >= b->cap) {
        return RS_ERR_FORMAT;
    }
    n = b->pos - start;
    if (n + 1 > dst_cap) {
        return RS_ERR_RANGE;
    }
    memcpy(dst, b->data + start, n);
    dst[n] = 0;
    b->pos++;
    return RS_OK;
}

int rs_g1_add128(rs_buf *b) {
    return (rs_g1(b) - 128) & 0xff;
}

int rs_g1_neg(rs_buf *b) {
    return -rs_g1b(b) & 0xff;
}

int rs_g1_sub128(rs_buf *b) {
    return (128 - rs_g1(b)) & 0xff;
}

int rs_g2_le(rs_buf *b) {
    if (!rs_buf_has(b, 2)) {
        return 0;
    }
    b->pos += 2;
    return (b->data[b->pos - 1] << 8) + (b->data[b->pos - 2] & 0xff);
}

int rs_g2_add128(rs_buf *b) {
    if (!rs_buf_has(b, 2)) {
        return 0;
    }
    b->pos += 2;
    return ((b->data[b->pos - 2] & 0xff) << 8) + ((b->data[b->pos - 1] - 128) & 0xff);
}

int rs_g2_le_add128(rs_buf *b) {
    if (!rs_buf_has(b, 2)) {
        return 0;
    }
    b->pos += 2;
    return ((b->data[b->pos - 1] & 0xff) << 8) + ((b->data[b->pos - 2] - 128) & 0xff);
}

int rs_g2b_le(rs_buf *b) {
    int v = rs_g2_le(b);
    if (v > 32767) {
        v -= 65536;
    }
    return v;
}

int rs_g4_alt2(rs_buf *b) {
    if (!rs_buf_has(b, 4)) {
        return 0;
    }
    b->pos += 4;
    return ((b->data[b->pos - 2] & 0xff) << 24) + ((b->data[b->pos - 1] & 0xff) << 16) +
           ((b->data[b->pos - 4] & 0xff) << 8) + (b->data[b->pos - 3] & 0xff);
}

int rs_g4_alt3(rs_buf *b) {
    if (!rs_buf_has(b, 4)) {
        return 0;
    }
    b->pos += 4;
    return ((b->data[b->pos - 3] & 0xff) << 24) + ((b->data[b->pos - 4] & 0xff) << 16) +
           ((b->data[b->pos - 1] & 0xff) << 8) + (b->data[b->pos - 2] & 0xff);
}

int rs_ig4(rs_buf *b) {
    if (!rs_buf_has(b, 4)) {
        return 0;
    }
    b->pos += 4;
    return (b->data[b->pos - 4] & 0xff) | ((b->data[b->pos - 3] & 0xff) << 8) |
           ((b->data[b->pos - 2] & 0xff) << 16) | ((b->data[b->pos - 1] & 0xff) << 24);
}
