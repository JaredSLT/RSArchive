#include "rslib/client/client.h"

#define RS_CLIENT_SEND(fn, pkt, enc, ty) \
rs_err fn(rs_client *c, const ty *v) { \
    uint8_t raw[4096]; \
    rs_buf b; \
    rs_err e; \
    if (!c || !v) return RS_ERR_ARG; \
    rs_buf_wrap(&b, raw, sizeof(raw)); \
    e = enc(rs_client_session(c), &b, v); \
    if (e != RS_OK) return e; \
    return rs_client_send_pkt(c, pkt, b.data, b.pos); \
}

RS_CLIENT_SEND(rs_client_send_walk, RS_C2S_WALK, rs_codec_walk_encode, rs_walk)
RS_CLIENT_SEND(rs_client_send_chat, RS_C2S_CHAT, rs_codec_chat_in_encode, rs_chat_in)
RS_CLIENT_SEND(rs_client_send_command, RS_C2S_COMMAND, rs_codec_command_encode, rs_command)
RS_CLIENT_SEND(rs_client_send_private, RS_C2S_PRIVATE_MESSAGE, rs_codec_private_out_encode, rs_private_out)
RS_CLIENT_SEND(rs_client_send_chat_mode, RS_C2S_CHAT_MODE, rs_codec_chat_mode_encode, rs_chat_mode)
RS_CLIENT_SEND(rs_client_send_design, RS_C2S_APPEARANCE, rs_codec_design_encode, rs_design)

#undef RS_CLIENT_SEND
