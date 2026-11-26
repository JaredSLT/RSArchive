#include "proto/ops.h"

rs_err rs_frame_read(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload, int use_c2s) {
    size_t start;
    int op, size, hdr;
    rs_isaac saved;
    int isaac_was_ready;
    if (!s || !src || !opcode || !payload) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 1) {
        return RS_ERR_NEED_MORE;
    }
    start = src->pos;
    isaac_was_ready = s->isaac_ready;
    saved = s->isaac_in;
    op = rs_session_decode_opcode(s, rs_g1(src));
    size = use_c2s ? rs_session_size_c2s(s, op) : rs_session_size_s2c(s, op);
    hdr = 0;
    if (size == -1) {
        if (rs_buf_remaining(src) < 1) {
            src->pos = start;
            s->isaac_in = saved;
            s->isaac_ready = isaac_was_ready;
            return RS_ERR_NEED_MORE;
        }
        size = rs_g1(src);
        hdr = 1;
    } else if (size == -2) {
        if (rs_buf_remaining(src) < 2) {
            src->pos = start;
            s->isaac_in = saved;
            s->isaac_ready = isaac_was_ready;
            return RS_ERR_NEED_MORE;
        }
        size = rs_g2(src);
        hdr = 2;
    } else if (size < 0) {
        src->pos = start;
        s->isaac_in = saved;
        s->isaac_ready = isaac_was_ready;
        return RS_ERR_PROTO;
    }
    if (rs_buf_remaining(src) < (size_t)size) {
        src->pos = start;
        s->isaac_in = saved;
        s->isaac_ready = isaac_was_ready;
        return RS_ERR_NEED_MORE;
    }
    rs_buf_wrap(payload, src->data + src->pos, (size_t)size);
    src->pos += (size_t)size;
    *opcode = op;
    (void)hdr;
    return RS_OK;
}

rs_err rs_frame_write(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload, size_t n,
                      int use_s2c) {
    rs_err e;
    int size;
    if (!s || !dst) {
        return RS_ERR_ARG;
    }
    e = rs_p1(dst, rs_session_encode_opcode(s, opcode));
    if (e != RS_OK) {
        return e;
    }
    size = use_s2c ? rs_session_size_s2c(s, opcode) : rs_session_size_c2s(s, opcode);
    if (size == -1) {
        e = rs_p1(dst, (int)n);
    } else if (size == -2) {
        e = rs_p2(dst, (int)n);
    } else if (size < 0) {
        return RS_ERR_PROTO;
    }
    if (e != RS_OK) {
        return e;
    }
    if (n && payload) {
        e = rs_pdata(dst, payload, n);
    }
    return e;
}
