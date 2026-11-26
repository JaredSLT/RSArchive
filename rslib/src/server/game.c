#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"

rs_err rs_server_handle_game(rs_conn *c) {
    for (;;) {
        rs_buf src, payload;
        rs_pkt id;
        int opcode;
        rs_err e;
        size_t used;

        rs_buf_wrap(&src, c->in.data, c->in.pos);
        e = rs_session_read_pkt(&c->session, &src, &id, &opcode, &payload);
        if (e == RS_ERR_NEED_MORE) {
            return RS_OK;
        }
        if (e != RS_OK) {
            return e;
        }
        used = src.pos;
        if (c->srv->hooks.on_packet) {
            c->srv->hooks.on_packet(c, id, opcode, &payload);
        }
        memmove(c->in.data, c->in.data + used, c->in.pos - used);
        c->in.pos -= used;
    }
}
