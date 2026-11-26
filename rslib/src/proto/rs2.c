#include "proto/ops.h"

rs_err rs2_read_game(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload) {
    int incoming_c2s = !s || s->role != RS_ROLE_CLIENT;
    return rs_frame_read(s, src, opcode, payload, incoming_c2s);
}

rs_err rs2_write_game(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload,
                             size_t n) {
    int outgoing_s2c = !s || s->role != RS_ROLE_CLIENT;
    return rs_frame_write(s, dst, opcode, payload, n, outgoing_s2c);
}

const rs_proto_ops rs_ops_rs2 = {
    "rs2",
    rs2_handshake_parse,
    rs2_handshake_write,
    rs2_handshake_reply,
    rs2_handshake_reply_parse,
    rs2_login_parse,
    rs2_login_write,
    rs2_login_success,
    rs2_login_success_parse,
    rs2_read_game,
    rs2_write_game,
    rs2_update_parse,
    rs2_update_write,
    rs2_update_reply,
};
