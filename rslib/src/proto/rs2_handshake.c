#include "proto/ops.h"

rs_err rs2_handshake_parse(const rs_build_info *hint, const uint8_t *data, size_t len,
                                  rs_handshake *out) {
    return rs_handshake_parse(hint, data, len, out);
}

rs_err rs2_handshake_write(rs_session *s, rs_buf *dst, rs_link link, int name_hash) {
    rs_err e;
    (void)s;
    if (link == RS_LINK_LOGIN) {
        e = rs_p1(dst, 14);
        if (e) {
            return e;
        }
        return rs_p1(dst, name_hash & 0xff);
    }
    if (link == RS_LINK_ONDEMAND) {
        return rs_p1(dst, 15);
    }
    return RS_ERR_PROTO;
}

rs_err rs2_handshake_reply(rs_session *s, rs_buf *dst, const rs_handshake *hs,
                                  uint64_t server_seed) {
    int i;
    rs_err e;
    (void)s;
    if (hs->link == RS_LINK_LOGIN) {
        return rs_login_write_handshake_ok(dst, server_seed);
    }
    if (hs->link == RS_LINK_ONDEMAND) {
        for (i = 0; i < 8; i++) {
            e = rs_p1(dst, 0);
            if (e != RS_OK) {
                return e;
            }
        }
        return RS_OK;
    }
    return RS_ERR_PROTO;
}

rs_err rs2_handshake_reply_parse(rs_session *s, rs_buf *src, rs_link link, int *status,
                                        uint64_t *server_seed) {
    int i;
    (void)s;
    if (link == RS_LINK_LOGIN) {
        if (rs_buf_remaining(src) < 17) {
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
    if (link == RS_LINK_ONDEMAND) {
        if (rs_buf_remaining(src) < 8) {
            return RS_ERR_NEED_MORE;
        }
        src->pos += 8;
        if (status) {
            *status = 0;
        }
        return RS_OK;
    }
    return RS_ERR_PROTO;
}
