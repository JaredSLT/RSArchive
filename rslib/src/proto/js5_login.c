#include "proto/ops.h"

rs_err js5_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa,
                              rs_login_request *out) {
    return rs_rs2_login_parse(s ? s->build : NULL, src, rsa, out);
}

rs_err js5_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req,
                              const rs_rsa_key *rsa) {
    return rs_rs2_login_write(s ? s->build : NULL, dst, req, rsa);
}

rs_err js5_login_success(rs_session *s, rs_buf *dst, int rights, int flagged) {
    return rs_login_write_success_ex(dst, s ? s->build : NULL, rights, flagged, 1);
}

rs_err js5_login_success_parse(rs_session *s, rs_buf *src, int *status, int *rights,
                                      int *flagged) {
    int st, extra;
    const rs_build_info *b = s ? s->build : NULL;
    if (rs_buf_remaining(src) < 1) {
        return RS_ERR_NEED_MORE;
    }
    st = rs_g1(src);
    if (status) {
        *status = st;
    }
    if (st != RS_LOGIN_OK && st != RS_LOGIN_RECONNECT_OK) {
        if (rights) {
            *rights = 0;
        }
        if (flagged) {
            *flagged = 0;
        }
        return RS_OK;
    }
    if (rs_buf_remaining(src) < 2) {
        return RS_ERR_NEED_MORE;
    }
    if (rights) {
        *rights = rs_g1(src);
    } else {
        (void)rs_g1(src);
    }
    if (flagged) {
        *flagged = rs_g1(src);
    } else {
        (void)rs_g1(src);
    }
    extra = 0;
    if (b) {
        extra = b->login_success_extra;
        if (b->login_player_index && extra < 2) {
            extra = 2;
        }
    }
    if (rs_buf_remaining(src) < (size_t)extra) {
        return RS_ERR_NEED_MORE;
    }
    if (b && b->login_player_index && extra >= 2) {
        int idx = rs_g2(src);
        extra -= 2;
        if (s) {
            s->player_index = idx;
        }
    }
    while (extra-- > 0) {
        (void)rs_g1(src);
    }
    return RS_OK;
}
