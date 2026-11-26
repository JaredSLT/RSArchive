#include "proto/ops.h"

rs_err rs_rsc_read(rs_buf *src, int *opcode, rs_buf *payload) {
    size_t start, body;
    int len;
    uint8_t *p;
    if (rs_buf_remaining(src) < 1) {
        return RS_ERR_NEED_MORE;
    }
    start = src->pos;
    len = rs_g1(src);
    if (len >= 160) {
        if (rs_buf_remaining(src) < 1) {
            src->pos = start;
            return RS_ERR_NEED_MORE;
        }
        len = (len - 160) * 256 + rs_g1(src);
        if (rs_buf_remaining(src) < (size_t)len) {
            src->pos = start;
            return RS_ERR_NEED_MORE;
        }
        if (len < 1) {
            src->pos = start;
            return RS_ERR_PROTO;
        }
        *opcode = src->data[src->pos] & 0xff;
        rs_buf_wrap(payload, src->data + src->pos + 1, (size_t)len - 1);
        src->pos += (size_t)len;
        return RS_OK;
    }
    if (rs_buf_remaining(src) < (size_t)len) {
        src->pos = start;
        return RS_ERR_NEED_MORE;
    }
    p = src->data + src->pos;
    body = (size_t)len;
    if (body == 0) {
        src->pos = start;
        return RS_ERR_PROTO;
    }
    if (body == 1) {
        *opcode = p[0] & 0xff;
        rs_buf_wrap(payload, p, 0);
    } else {
        /* Stream.readPacket: first byte goes to buf[len-1], the rest to buf[0]. */
        uint8_t last = p[0];
        memmove(p, p + 1, body - 1);
        p[body - 1] = last;
        *opcode = p[0] & 0xff;
        rs_buf_wrap(payload, p + 1, body - 1);
    }
    src->pos += body;
    return RS_OK;
}

rs_err rs_rsc_write(rs_buf *dst, int opcode, const uint8_t *payload, size_t n) {
    size_t total = n + 1;
    rs_err e;
    if (total >= 160) {
        e = rs_p1(dst, (int)(160 + total / 256));
        if (e) {
            return e;
        }
        e = rs_p1(dst, (int)(total & 255));
        if (e) {
            return e;
        }
        e = rs_p1(dst, opcode);
        if (e) {
            return e;
        }
        if (n && payload) {
            return rs_pdata(dst, payload, n);
        }
        return RS_OK;
    }
    e = rs_p1(dst, (int)total);
    if (e) {
        return e;
    }
    if (n == 0) {
        return rs_p1(dst, opcode);
    }
    e = rs_p1(dst, payload[n - 1]);
    if (e) {
        return e;
    }
    e = rs_p1(dst, opcode);
    if (e) {
        return e;
    }
    if (n > 1) {
        return rs_pdata(dst, payload, n - 1);
    }
    return RS_OK;
}

static rs_err rsc_handshake_parse(const rs_build_info *hint, const uint8_t *data, size_t len,
                                  rs_handshake *out) {
    (void)data;
    (void)len;
    memset(out, 0, sizeof(*out));
    out->link = RS_LINK_LOGIN;
    out->revision = hint ? hint->revision : 0;
    out->consumed = 0;
    return RS_ERR_NEED_MORE;
}

static rs_err rsc_handshake_write(rs_session *s, rs_buf *dst, rs_link link, int name_hash) {
    (void)s;
    (void)dst;
    (void)link;
    (void)name_hash;
    return RS_OK;
}

static rs_err rsc_handshake_reply(rs_session *s, rs_buf *dst, const rs_handshake *hs,
                                  uint64_t server_seed) {
    (void)s;
    (void)hs;
    return rs_p4(dst, (int)(server_seed & 0xffffffffu));
}

static rs_err rsc_handshake_reply_parse(rs_session *s, rs_buf *src, rs_link link, int *status,
                                        uint64_t *server_seed) {
    (void)link;
    if (rs_buf_remaining(src) < 4) {
        return RS_ERR_NEED_MORE;
    }
    if (server_seed) {
        *server_seed = (uint64_t)(uint32_t)rs_g4(src);
    } else {
        src->pos += 4;
    }
    if (s) {
        s->server_seed = server_seed ? *server_seed : 0;
    }
    if (status) {
        *status = 0;
    }
    return RS_OK;
}

static rs_err rsc_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa,
                              rs_login_request *out) {
    rs_buf payload;
    int opcode;
    rs_err e;
    memset(out, 0, sizeof(*out));
    e = rs_rsc_read(src, &opcode, &payload);
    if (e != RS_OK) {
        return e;
    }
    if (s && s->build && s->build->revision == 177) {
        /* First packet: encodeKey 3141592, low 8 bits 216. */
        opcode = (opcode - 216) & 255;
    }
    out->reconnect = (opcode == 19);
    out->revision = s && s->build ? s->build->revision : 0;
    if (payload.cap >= 2) {
        out->revision = (payload.data[0] << 8) | payload.data[1];
        payload.pos = 2;
        if (out->revision != 115 && rs_buf_remaining(&payload) >= 2) {
            (void)rs_g2(&payload); /* referid, absent on 115 */
        }
        if (rs_buf_remaining(&payload) >= 8) {
            out->username37 = rs_g8(&payload);
            rs_base37_decode(out->username37, out->username);
        }
        if (rsa && rsa->dlen > 0 && rs_buf_remaining(&payload) > 0) {
            uint8_t plain[RS_RSA_MAX_BYTES];
            size_t plen = 0;
            int n = (int)rs_buf_remaining(&payload);
            if (n > RS_RSA_MAX_BYTES) {
                n = RS_RSA_MAX_BYTES;
            }
            e = rs_rsa_modpow_key(rsa, 1, payload.data + payload.pos, (size_t)n, plain, &plen);
            if (e != RS_OK) {
                return e;
            }
            if (plen >= sizeof(out->password)) {
                plen = sizeof(out->password) - 1;
            }
            memcpy(out->password, plain, plen);
            out->password[plen] = 0;
        } else if (rs_buf_remaining(&payload) > 0) {
            rs_gjstr(&payload, out->password, sizeof(out->password));
        }
        if (rs_buf_remaining(&payload) >= 4) {
            out->uid = rs_g4s(&payload);
        }
    }
    return RS_OK;
}

static rs_err rsc_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req,
                              const rs_rsa_key *rsa) {
    uint8_t body[160];
    rs_buf b;
    rs_err e;
    int opcode, rev;
    uint64_t u37;
    if (!req) {
        return RS_ERR_ARG;
    }
    rs_buf_wrap(&b, body, sizeof(body));
    rev = req->revision ? req->revision : (s && s->build ? s->build->revision : 177);
    e = rs_p2(&b, rev);
    if (e) {
        return e;
    }
    if (rev != 115) {
        e = rs_p2(&b, 0);
        if (e) {
            return e;
        }
    }
    u37 = req->username37 ? req->username37 : rs_base37_encode(req->username);
    e = rs_p8(&b, u37);
    if (e) {
        return e;
    }
    if (rsa && rsa->elen > 0) {
        uint8_t enc[RS_RSA_MAX_BYTES];
        size_t enc_len = 0;
        e = rs_rsa_modpow(rsa, (const uint8_t *)req->password, strlen(req->password), enc, &enc_len);
        if (e) {
            return e;
        }
        e = rs_pdata(&b, enc, enc_len);
        if (e) {
            return e;
        }
    } else {
        e = rs_pjstr(&b, req->password);
        if (e) {
            return e;
        }
    }
    e = rs_p4(&b, req->uid);
    if (e) {
        return e;
    }
    opcode = req->reconnect ? 19 : 0;
    if (rev == 177) {
        opcode = (opcode + 216) & 255;
    }
    return rs_rsc_write(dst, opcode, body, b.pos);
}

static rs_err rsc_login_success(rs_session *s, rs_buf *dst, int rights, int flagged) {
    int rev = s && s->build ? s->build->revision : 0;
    int status = 0;
    (void)rights;
    (void)flagged;
    /* Login inlines two reads: short-packet length, then the status byte. */
    if (rev == 177) {
        status = (status + 216) & 255;
    }
    return rs_rsc_write(dst, status, NULL, 0);
}

static rs_err rsc_login_success_parse(rs_session *s, rs_buf *src, int *status, int *rights,
                                      int *flagged) {
    rs_buf payload;
    int opcode;
    rs_err e = rs_rsc_read(src, &opcode, &payload);
    if (e != RS_OK) {
        return e;
    }
    if (s && s->build && s->build->revision == 177) {
        opcode = (opcode - 216) & 255;
    }
    if (status) {
        *status = opcode;
    }
    if (rights) {
        *rights = 0;
    }
    if (flagged) {
        *flagged = 0;
    }
    return RS_OK;
}

static rs_err rsc_read_game(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload) {
    (void)s;
    return rs_rsc_read(src, opcode, payload);
}

static rs_err rsc_write_game(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload,
                             size_t n) {
    (void)s;
    return rs_rsc_write(dst, opcode, payload, n);
}

static rs_err rsc_unsupported_update(rs_session *s, const uint8_t *data, size_t len,
                                     rs_update_req *out, size_t *consumed) {
    (void)s;
    (void)data;
    (void)len;
    (void)out;
    (void)consumed;
    return RS_ERR_UNSUPPORTED;
}

static rs_err rsc_unsupported_update_write(rs_session *s, rs_buf *dst, const rs_update_req *req) {
    (void)s;
    (void)dst;
    (void)req;
    return RS_ERR_UNSUPPORTED;
}

static rs_err rsc_unsupported_update_reply(rs_session *s, rs_buf *dst, const rs_update_req *req,
                                           const uint8_t *data, size_t len) {
    (void)s;
    (void)dst;
    (void)req;
    (void)data;
    (void)len;
    return RS_ERR_UNSUPPORTED;
}

const rs_proto_ops rs_ops_rsc = {
    "rsc",
    rsc_handshake_parse,
    rsc_handshake_write,
    rsc_handshake_reply,
    rsc_handshake_reply_parse,
    rsc_login_parse,
    rsc_login_write,
    rsc_login_success,
    rsc_login_success_parse,
    rsc_read_game,
    rsc_write_game,
    rsc_unsupported_update,
    rsc_unsupported_update_write,
    rsc_unsupported_update_reply,
};
