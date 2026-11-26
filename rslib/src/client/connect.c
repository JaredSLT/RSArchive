#define _POSIX_C_SOURCE 200809L

#include "client/client_impl.h"

rs_err rs_client_connect(rs_client *c, const char *host, int port) {
    rs_sock fd;
    const rs_build_info *b;
    if (!c || !host) {
        return RS_ERR_ARG;
    }
    if (port <= 0) {
        b = rs_ctx_build(c->ctx);
        port = b && b->default_port ? b->default_port : 43594;
    }
    fd = rs_sock_connect(host, port);
    if (fd == RS_INVALID_SOCK) {
        return RS_ERR_IO;
    }
    if (c->fd >= 0) {
        rs_sock_close((rs_sock)c->fd);
    }
    rs_buf_free(&c->in);
    rs_buf_free(&c->out);
    if (rs_buf_alloc(&c->in, 512) != RS_OK || rs_buf_alloc(&c->out, 512) != RS_OK) {
        rs_sock_close(fd);
        return RS_ERR_NOMEM;
    }
    c->fd = (int)fd;
    c->got_hs_reply = 0;
    c->got_login_reply = 0;
    c->closing = 0;
    c->running = 1;
    c->next_tick_ms = rs_now_ms() + (uint64_t)c->tick_ms;
    rs_session_init_role(&c->session, rs_ctx_build(c->ctx), RS_ROLE_CLIENT);
    if (c->hooks.on_connect) {
        c->hooks.on_connect(c);
    }
    return RS_OK;
}
