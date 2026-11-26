#include "proto/ops.h"

rs_err rs_rsa_block_parse(rs_buf *block, rs_login_request *out) {
    return rs_rsa_block_parse_ex(block, out, 0);
}

rs_err rs_rsa_block_parse_ex(rs_buf *block, rs_login_request *out, int user37) {
    int magic = rs_g1(block);
    int i;
    if (magic != 10) {
        return RS_ERR_PROTO;
    }
    for (i = 0; i < 4; i++) {
        out->isaac_seed[i] = (uint32_t)rs_g4(block);
    }
    if (user37) {
        out->username37 = rs_g8(block);
        rs_base37_decode(out->username37, out->username);
        if (rs_gjstr(block, out->password, sizeof(out->password)) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        return RS_OK;
    }
    out->uid = rs_g4s(block);
    if (rs_gjstr(block, out->username, sizeof(out->username)) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    if (rs_gjstr(block, out->password, sizeof(out->password)) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

rs_err rs_login_write_handshake_ok(rs_buf *dst, uint64_t server_seed) {
    int i;
    rs_err e;
    for (i = 0; i < 8; i++) {
        e = rs_p1(dst, 0);
        if (e != RS_OK) {
            return e;
        }
    }
    e = rs_p1(dst, 0);
    if (e != RS_OK) {
        return e;
    }
    return rs_p8(dst, server_seed);
}

rs_err rs_rs2_login_parse(const rs_build_info *build, rs_buf *src, const rs_rsa_key *rsa,
                          rs_login_request *out) {
    int opcode, size, wide, ncrc, user37, rsa_len;
    size_t start, payload_end;
    rs_buf rsa_buf;
    uint8_t rsa_in[RS_RSA_MAX_BYTES];
    uint8_t rsa_out[RS_RSA_MAX_BYTES];
    size_t rsa_out_len;
    rs_err e;

    memset(out, 0, sizeof(*out));
    if (!src) {
        return RS_ERR_ARG;
    }
    wide = build && build->login_size_wide;
    if (rs_buf_remaining(src) < (wide ? 3u : 2u)) {
        return RS_ERR_NEED_MORE;
    }
    start = src->pos;
    opcode = rs_g1(src);
    if (opcode != 16 && opcode != 18) {
        if (!build || (opcode != build->login_new && opcode != build->login_reconnect)) {
            src->pos = start;
            return RS_ERR_PROTO;
        }
    }
    out->reconnect = (opcode == 18 || (build && opcode == build->login_reconnect));
    size = wide ? rs_g2(src) : rs_g1(src);
    if (size < 0 || rs_buf_remaining(src) < (size_t)size) {
        src->pos = start;
        return RS_ERR_NEED_MORE;
    }
    payload_end = src->pos + (size_t)size;
    if (!wide) {
        if (build && build->revision == 254) {
            /* Client.login writes one revision byte and no 255 marker. */
            out->revision = rs_g1(src);
            out->low_memory = rs_g1(src);
        } else {
            (void)rs_g1(src); /* 255 */
            out->revision = rs_g2(src);
            out->low_memory = rs_g1(src);
        }
    } else if (build && build->login_rev_bytes >= 4) {
        out->revision = rs_g4s(src);
        out->low_memory = rs_buf_remaining(src) ? rs_g1(src) : 0;
    } else {
        out->revision = rs_g2(src);
        out->low_memory = rs_buf_remaining(src) ? rs_g1(src) : 0;
    }
    ncrc = rs_login_crc_count(build);
    user37 = build && build->login_user37;
    while (src->pos + 1 < payload_end) {
        int n = src->data[src->pos] & 0xff;
        if (n >= 10 && n <= RS_RSA_MAX_BYTES && src->pos + 1 + (size_t)n == payload_end) {
            break;
        }
        if (out->ncrc < ncrc && src->pos + 4 <= payload_end) {
            out->archive_crc[out->ncrc++] = (uint32_t)rs_g4(src);
        } else if (src->pos + 1 < payload_end) {
            src->pos++;
        } else {
            break;
        }
    }
    if (src->pos >= payload_end) {
        src->pos = payload_end;
        return RS_ERR_PROTO;
    }
    rsa_len = rs_g1(src);
    if (rsa_len < 1 || src->pos + (size_t)rsa_len > payload_end) {
        src->pos = payload_end;
        return RS_ERR_PROTO;
    }
    if (rsa && rsa->dlen > 0) {
        if (rsa_len > RS_RSA_MAX_BYTES) {
            src->pos = payload_end;
            return RS_ERR_PROTO;
        }
        memcpy(rsa_in, src->data + src->pos, (size_t)rsa_len);
        src->pos += (size_t)rsa_len;
        e = rs_rsa_modpow_key(rsa, 1, rsa_in, (size_t)rsa_len, rsa_out, &rsa_out_len);
        if (e != RS_OK) {
            src->pos = payload_end;
            return e;
        }
        rs_buf_wrap(&rsa_buf, rsa_out, rsa_out_len);
        e = rs_rsa_block_parse_ex(&rsa_buf, out, user37);
        src->pos = payload_end;
        return e;
    }
    rs_buf_wrap(&rsa_buf, src->data + src->pos, (size_t)rsa_len);
    src->pos += (size_t)rsa_len;
    e = rs_rsa_block_parse_ex(&rsa_buf, out, user37);
    src->pos = payload_end;
    return e;
}

rs_err rs_login_parse(const rs_build_info *build, rs_buf *src, const rs_rsa_key *rsa,
                      rs_login_request *out) {
    return rs_rs2_login_parse(build, src, rsa, out);
}

rs_err rs_rs2_login_write(const rs_build_info *build, rs_buf *dst, const rs_login_request *req,
                          const rs_rsa_key *rsa) {
    size_t size_at;
    rs_err e;
    uint8_t block[256];
    rs_buf blk;
    int i, opcode, ncrc, wide, user37;
    uint8_t enc[RS_RSA_MAX_BYTES];
    size_t enc_len = 0;
    int rev;

    if (!dst || !req) {
        return RS_ERR_ARG;
    }
    wide = build && build->login_size_wide;
    user37 = build && build->login_user37;
    ncrc = rs_login_crc_count(build);
    opcode = req->reconnect ? (build && build->login_reconnect ? build->login_reconnect : 18)
                            : (build && build->login_new ? build->login_new : 16);
    e = rs_p1(dst, opcode);
    if (e) {
        return e;
    }
    size_at = dst->pos;
    e = wide ? rs_p2(dst, 0) : rs_p1(dst, 0);
    if (e) {
        return e;
    }
    rev = req->revision ? req->revision : (build ? build->revision : 0);
    if (!wide) {
        if (rev == 254) {
            e = rs_p1(dst, rev);
            if (e) {
                return e;
            }
        } else {
            e = rs_p1(dst, 255);
            if (e) {
                return e;
            }
            e = rs_p2(dst, rev);
            if (e) {
                return e;
            }
        }
        e = rs_p1(dst, req->low_memory);
        if (e) {
            return e;
        }
    } else if (build && build->login_rev_bytes >= 4) {
        e = rs_p4(dst, rev);
        if (e) {
            return e;
        }
        e = rs_p1(dst, req->low_memory);
        if (e) {
            return e;
        }
    } else {
        e = rs_p2(dst, rev);
        if (e) {
            return e;
        }
        e = rs_p1(dst, req->low_memory);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < ncrc; i++) {
        e = rs_p4(dst, (int)req->archive_crc[i]);
        if (e) {
            return e;
        }
    }
    rs_buf_wrap(&blk, block, sizeof(block));
    e = rs_p1(&blk, 10);
    if (e) {
        return e;
    }
    for (i = 0; i < 4; i++) {
        e = rs_p4(&blk, (int)req->isaac_seed[i]);
        if (e) {
            return e;
        }
    }
    if (user37) {
        uint64_t u = req->username37 ? req->username37 : rs_base37_encode(req->username);
        e = rs_p8(&blk, u);
        if (e) {
            return e;
        }
        e = rs_pjstr(&blk, req->password);
        if (e) {
            return e;
        }
    } else {
        e = rs_p4(&blk, req->uid);
        if (e) {
            return e;
        }
        e = rs_pjstr(&blk, req->username);
        if (e) {
            return e;
        }
        e = rs_pjstr(&blk, req->password);
        if (e) {
            return e;
        }
    }
    if (rsa && rsa->elen > 0) {
        e = rs_rsa_modpow(rsa, block, blk.pos, enc, &enc_len);
        if (e) {
            return e;
        }
        e = rs_p1(dst, (int)enc_len);
        if (e) {
            return e;
        }
        e = rs_pdata(dst, enc, enc_len);
    } else {
        e = rs_p1(dst, (int)blk.pos);
        if (e) {
            return e;
        }
        e = rs_pdata(dst, block, blk.pos);
    }
    if (e) {
        return e;
    }
    return wide ? rs_psize2(dst, size_at) : rs_psize1(dst, size_at);
}

rs_err rs_login_write_success(rs_buf *dst, int rights, int flagged) {
    return rs_login_write_success_ex(dst, NULL, rights, flagged, 1);
}

rs_err rs_login_write_success_ex(rs_buf *dst, const rs_build_info *build, int rights, int flagged,
                                 int player_index) {
    rs_err e = rs_p1(dst, RS_LOGIN_OK);
    if (e != RS_OK) {
        return e;
    }
    e = rs_p1(dst, rights);
    if (e != RS_OK) {
        return e;
    }
    e = rs_p1(dst, flagged ? 1 : 0);
    if (e != RS_OK) {
        return e;
    }
    if (build && build->login_player_index) {
        e = rs_p2(dst, player_index);
        if (e != RS_OK) {
            return e;
        }
    }
    if (build && build->login_success_extra > 0) {
        int extra = build->login_success_extra;
        if (build->login_player_index) {
            extra -= 2;
        }
        while (extra-- > 0) {
            e = rs_p1(dst, 0);
            if (e != RS_OK) {
                return e;
            }
        }
    }
    return e;
}

rs_err rs2_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa,
                              rs_login_request *out) {
    return rs_rs2_login_parse(s ? s->build : NULL, src, rsa, out);
}

rs_err rs2_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req,
                              const rs_rsa_key *rsa) {
    return rs_rs2_login_write(s ? s->build : NULL, dst, req, rsa);
}

rs_err rs2_login_success(rs_session *s, rs_buf *dst, int rights, int flagged) {
    return rs_login_write_success_ex(dst, s ? s->build : NULL, rights, flagged, 1);
}

rs_err rs2_login_success_parse(rs_session *s, rs_buf *src, int *status, int *rights,
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
    if (extra <= 0) {
        return RS_OK;
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
