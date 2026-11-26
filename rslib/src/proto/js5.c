#include "proto/ops.h"

rs_err js5_read_game(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload) {
    int incoming_c2s = !s || s->role != RS_ROLE_CLIENT;
    return rs_frame_read(s, src, opcode, payload, incoming_c2s);
}

rs_err js5_write_game(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload,
                             size_t n) {
    int outgoing_s2c = !s || s->role != RS_ROLE_CLIENT;
    return rs_frame_write(s, dst, opcode, payload, n, outgoing_s2c);
}

const rs_proto_ops rs_ops_js5 = {
    "js5",
    js5_handshake_parse,
    js5_handshake_write,
    js5_handshake_reply,
    js5_handshake_reply_parse,
    js5_login_parse,
    js5_login_write,
    js5_login_success,
    js5_login_success_parse,
    js5_read_game,
    js5_write_game,
    js5_update_parse,
    js5_update_write,
    js5_update_reply,
};

rs_err js5_lobby_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa, rs_login_request *out);
rs_err js5_lobby_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req,
                             const rs_rsa_key *rsa);

const rs_proto_ops rs_ops_js5_lobby = {
    "js5-lobby",
    js5_handshake_parse,
    js5_handshake_write,
    js5_handshake_reply,
    js5_handshake_reply_parse,
    js5_lobby_login_parse,
    js5_lobby_login_write,
    js5_login_success,
    js5_login_success_parse,
    js5_read_game,
    js5_write_game,
    js5_update_parse,
    js5_update_write,
    js5_update_reply,
};
