#define _POSIX_C_SOURCE 200809L

#include "client/client_impl.h"

rs_err rs_client_handshake(rs_client *c, rs_link link, int name_hash) {
    uint8_t tmp[16];
    rs_buf b;
    rs_err e;
    if (!c || c->fd < 0) {
        return RS_ERR_ARG;
    }
    rs_buf_wrap(&b, tmp, sizeof(tmp));
    e = rs_session_handshake_write(&c->session, &b, link, name_hash);
    if (e != RS_OK) {
        return e;
    }
    c->hs.link = link;
    c->hs.opcode = link == RS_LINK_JS5 ? 15 : 14;
    c->session.link = link;
    c->got_hs_reply = 0;
    return rs_client_send(c, tmp, b.pos);
}
