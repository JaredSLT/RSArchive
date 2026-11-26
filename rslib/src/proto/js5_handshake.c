#include "proto/ops.h"

rs_err js5_handshake_parse(const rs_build_info *hint, const uint8_t *data, size_t len,
                                  rs_handshake *out) {
    return rs_handshake_parse(hint, data, len, out);
}

rs_err js5_handshake_write(rs_session *s, rs_buf *dst, rs_link link, int name_hash) {
    rs_err e;
    int rev = s && s->build ? s->build->revision : 0;
    if (link == RS_LINK_LOGIN) {
        e = rs_p1(dst, 14);
        if (e) {
            return e;
        }
        return rs_p1(dst, name_hash & 0xff);
    }
    if (link == RS_LINK_JS5) {
        e = rs_p1(dst, 15);
        if (e) {
            return e;
        }
        return rs_p4(dst, rev);
    }
    return RS_ERR_PROTO;
}

rs_err js5_handshake_reply(rs_session *s, rs_buf *dst, const rs_handshake *hs,
                                  uint64_t server_seed) {
    if (hs->link == RS_LINK_LOGIN) {
        return rs_login_write_handshake_ok(dst, server_seed);
    }
    if (hs->link == RS_LINK_JS5) {
        (void)s;
        return rs_js5_write_status(dst, 0);
    }
    return RS_ERR_PROTO;
}

rs_err js5_handshake_reply_parse(rs_session *s, rs_buf *src, rs_link link, int *status,
                                        uint64_t *server_seed) {
    if (link == RS_LINK_LOGIN) {
        int i;
        const rs_build_info *b = s ? s->build : NULL;
        size_t need = 17;
        if (b && b->revision >= 474) {
            /* still 8 dummy + status + seed at minimum */
        }
        if (rs_buf_remaining(src) < need) {
            return RS_ERR_NEED_MORE;
        }
        for (i = 0; i < 8; i++) {
            (void)rs_g1(src);
        }
        if (status) {
            *status = rs_g1(src);
        } else {
            (void)rs_g1(src);
        }
        if (server_seed) {
            *server_seed = rs_g8(src);
        } else {
            src->pos += 8;
        }
        return RS_OK;
    }
    if (link == RS_LINK_JS5) {
        if (rs_buf_remaining(src) < 1) {
            return RS_ERR_NEED_MORE;
        }
        if (status) {
            *status = rs_g1(src);
        } else {
            (void)rs_g1(src);
        }
        return RS_OK;
    }
    return RS_ERR_PROTO;
}
