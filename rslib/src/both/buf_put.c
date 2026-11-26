#include "both/buf_impl.h"

rs_err rs_p1(rs_buf *b, int v) {
    uint8_t x = (uint8_t)v;
    return rs_buf_putn(b, &x, 1);
}

rs_err rs_p2(rs_buf *b, int v) {
    uint8_t x[2];
    rs_put_be16(x, v);
    return rs_buf_putn(b, x, 2);
}

rs_err rs_ip2(rs_buf *b, int v) {
    uint8_t x[2] = {(uint8_t)v, (uint8_t)(v >> 8)};
    return rs_buf_putn(b, x, 2);
}

rs_err rs_p3(rs_buf *b, int v) {
    uint8_t x[3];
    rs_put_be24(x, v);
    return rs_buf_putn(b, x, 3);
}

rs_err rs_p4(rs_buf *b, int v) {
    uint8_t x[4];
    rs_put_be32(x, (uint32_t)v);
    return rs_buf_putn(b, x, 4);
}

rs_err rs_ip4(rs_buf *b, int v) {
    uint8_t x[4] = {(uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24)};
    return rs_buf_putn(b, x, 4);
}

rs_err rs_p5(rs_buf *b, uint64_t v) {
    uint8_t x[5] = {(uint8_t)(v >> 32), (uint8_t)(v >> 24), (uint8_t)(v >> 16),
                    (uint8_t)(v >> 8), (uint8_t)v};
    return rs_buf_putn(b, x, 5);
}

rs_err rs_p8(rs_buf *b, uint64_t v) {
    uint8_t x[8] = {(uint8_t)(v >> 56), (uint8_t)(v >> 48), (uint8_t)(v >> 40),
                    (uint8_t)(v >> 32), (uint8_t)(v >> 24), (uint8_t)(v >> 16),
                    (uint8_t)(v >> 8),  (uint8_t)v};
    return rs_buf_putn(b, x, 8);
}

rs_err rs_pdata(rs_buf *b, const void *src, size_t n) {
    return rs_buf_putn(b, src, n);
}

rs_err rs_pjstr(rs_buf *b, const char *s) {
    rs_err e = rs_pdata(b, s, strlen(s));
    if (e != RS_OK) {
        return e;
    }
    return rs_p1(b, 10);
}

rs_err rs_pjstrnull(rs_buf *b, const char *s) {
    rs_err e = rs_pdata(b, s, strlen(s));
    if (e != RS_OK) {
        return e;
    }
    return rs_p1(b, 0);
}

rs_err rs_psmarts(rs_buf *b, int v) {
    if (v < 0) {
        return RS_ERR_RANGE;
    }
    if (v < 128) {
        return rs_p1(b, v);
    }
    return rs_p2(b, v + 32768);
}

rs_err rs_psize1(rs_buf *b, size_t start) {
    size_t n = b->pos - start - 1;
    if (start >= b->pos) {
        return RS_ERR_RANGE;
    }
    b->data[start] = (uint8_t)n;
    return RS_OK;
}

rs_err rs_psize2(rs_buf *b, size_t start) {
    size_t n = b->pos - start - 2;
    if (start + 1 >= b->pos) {
        return RS_ERR_RANGE;
    }
    rs_put_be16(b->data + start, (int)n);
    return RS_OK;
}

rs_err rs_psize4(rs_buf *b, size_t start) {
    size_t n = b->pos - start - 4;
    if (start + 3 >= b->pos) {
        return RS_ERR_RANGE;
    }
    rs_put_be32(b->data + start, (uint32_t)n);
    return RS_OK;
}

rs_err rs_p1_neg(rs_buf *b, int v) {
    return rs_p1(b, -v);
}

rs_err rs_p1_sub128(rs_buf *b, int v) {
    return rs_p1(b, 128 - v);
}

rs_err rs_p1_add128(rs_buf *b, int v) {
    return rs_p1(b, v + 128);
}

rs_err rs_p2_le(rs_buf *b, int v) {
    return rs_ip2(b, v);
}

rs_err rs_p2_add128(rs_buf *b, int v) {
    rs_err e = rs_p1(b, v >> 8);
    if (e != RS_OK) {
        return e;
    }
    return rs_p1(b, v + 128);
}

rs_err rs_p2_le_add128(rs_buf *b, int v) {
    rs_err e = rs_p1(b, v + 128);
    if (e != RS_OK) {
        return e;
    }
    return rs_p1(b, v >> 8);
}

rs_err rs_p4_alt2(rs_buf *b, int v) {
    /* middle-endian: b2 b3 b0 b1 */
    rs_err e;
    e = rs_p1(b, v >> 8);
    if (e) return e;
    e = rs_p1(b, v);
    if (e) return e;
    e = rs_p1(b, v >> 24);
    if (e) return e;
    return rs_p1(b, v >> 16);
}

rs_err rs_p4_alt3(rs_buf *b, int v) {
    rs_err e;
    e = rs_p1(b, v >> 16);
    if (e) return e;
    e = rs_p1(b, v >> 24);
    if (e) return e;
    e = rs_p1(b, v);
    if (e) return e;
    return rs_p1(b, v >> 8);
}
