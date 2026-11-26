#include "proto/ops.h"

const rs_proto_ops *rs_proto_rsc(void) {
    return &rs_ops_rsc;
}

const rs_proto_ops *rs_proto_rs2(void) {
    return &rs_ops_rs2;
}

const rs_proto_ops *rs_proto_js5(void) {
    return &rs_ops_js5;
}

const rs_proto_ops *rs_proto_js5_lobby(void) {
    return &rs_ops_js5_lobby;
}

const rs_proto_ops *rs_proto_for(const rs_build_info *build) {
    if (build && build->ops) {
        return build->ops;
    }
    if (!build) {
        return &rs_ops_rs2;
    }
    if (build->era == RS_ERA_RSC) {
        return &rs_ops_rsc;
    }
    if (build->era == RS_ERA_JS5 || build->js5) {
        return &rs_ops_js5;
    }
    return &rs_ops_rs2;
}

rs_err rs_handshake_parse(const rs_build_info *hint, const uint8_t *data, size_t len,
                          rs_handshake *out) {
    int op;
    memset(out, 0, sizeof(*out));
    if (!data || len < 1) {
        return RS_ERR_NEED_MORE;
    }
    op = data[0] & 0xff;
    out->opcode = op;
    if (op == 14) {
        if (len < 2) {
            return RS_ERR_NEED_MORE;
        }
        out->link = RS_LINK_LOGIN;
        out->name_hash = data[1] & 0xff;
        out->consumed = 2;
        if (hint) {
            out->revision = hint->revision;
        }
        return RS_OK;
    }
    if (op == 15) {
        if (len >= 5) {
            int rev = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
            const rs_build_info *found = rs_build_lookup(rev);
            if ((found && found->js5) || (hint && hint->js5) || !hint || !hint->ondemand) {
                out->link = RS_LINK_JS5;
                out->revision = rev;
                out->consumed = 5;
                return RS_OK;
            }
        }
        if (hint && hint->ondemand && !hint->js5) {
            out->link = RS_LINK_ONDEMAND;
            out->revision = hint->revision;
            out->consumed = 1;
            return RS_OK;
        }
        return RS_ERR_NEED_MORE;
    }
    if (op == 16 || op == 18) {
        out->link = RS_LINK_LOGIN;
        out->consumed = 0;
        return RS_OK;
    }
    if (hint && hint->era == RS_ERA_RSC) {
        out->link = RS_LINK_LOGIN;
        out->revision = hint->revision;
        out->consumed = 0;
        return RS_OK;
    }
    return RS_ERR_PROTO;
}

static const rs_proto_ops *ops_of(const rs_session *s) {
    if (s && s->ops) {
        return s->ops;
    }
    return rs_proto_for(s ? s->build : NULL);
}

rs_err rs_session_read(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->read_game) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->read_game(s, src, opcode, payload);
}

rs_err rs_session_write(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload, size_t n) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->write_game) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->write_game(s, dst, opcode, payload, n);
}

rs_err rs_session_read_c2s(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload) {
    if (s && s->build && s->build->era == RS_ERA_RSC) {
        return rs_rsc_read(src, opcode, payload);
    }
    return rs_frame_read(s, src, opcode, payload, 1);
}

rs_err rs_session_write_s2c(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload,
                            size_t n) {
    if (s && s->build && s->build->era == RS_ERA_RSC) {
        return rs_rsc_write(dst, opcode, payload, n);
    }
    return rs_frame_write(s, dst, opcode, payload, n, 1);
}

rs_err rs_session_handshake_parse(rs_session *s, const uint8_t *data, size_t len, rs_handshake *out) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->handshake_parse) {
        return rs_handshake_parse(s ? s->build : NULL, data, len, out);
    }
    return ops->handshake_parse(s ? s->build : NULL, data, len, out);
}

rs_err rs_session_handshake_write(rs_session *s, rs_buf *dst, rs_link link, int name_hash) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->handshake_write) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->handshake_write(s, dst, link, name_hash);
}

rs_err rs_session_handshake_reply(rs_session *s, rs_buf *dst, const rs_handshake *hs,
                                  uint64_t server_seed) {
    const rs_proto_ops *ops = ops_of(s);
    if (!s || !dst || !hs) {
        return RS_ERR_ARG;
    }
    if (!ops || !ops->handshake_reply) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->handshake_reply(s, dst, hs, server_seed);
}

rs_err rs_session_handshake_reply_parse(rs_session *s, rs_buf *src, rs_link link, int *status,
                                        uint64_t *server_seed) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->handshake_reply_parse) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->handshake_reply_parse(s, src, link, status, server_seed);
}

rs_err rs_session_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa,
                              rs_login_request *out) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->login_parse) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->login_parse(s, src, rsa, out);
}

rs_err rs_session_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req,
                              const rs_rsa_key *rsa) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->login_write) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->login_write(s, dst, req, rsa);
}

rs_err rs_session_login_success(rs_session *s, rs_buf *dst, int rights, int flagged) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->login_success) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->login_success(s, dst, rights, flagged);
}

rs_err rs_session_login_success_parse(rs_session *s, rs_buf *src, int *status, int *rights,
                                      int *flagged) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->login_success_parse) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->login_success_parse(s, src, status, rights, flagged);
}

rs_err rs_session_update_parse(rs_session *s, const uint8_t *data, size_t len, rs_update_req *out,
                               size_t *consumed) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->update_parse) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->update_parse(s, data, len, out, consumed);
}

rs_err rs_session_update_write(rs_session *s, rs_buf *dst, const rs_update_req *req) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->update_write) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->update_write(s, dst, req);
}

rs_err rs_session_update_reply(rs_session *s, rs_buf *dst, const rs_update_req *req,
                               const uint8_t *data, size_t len) {
    const rs_proto_ops *ops = ops_of(s);
    if (!ops || !ops->update_reply) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->update_reply(s, dst, req, data, len);
}
