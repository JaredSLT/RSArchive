#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"
#include "both/sock_impl.h"

rs_err rs_server_accept(rs_server *s) {
    rs_sock fd;
    rs_conn *c;
    const rs_build_info *b;

    fd = rs_sock_accept((rs_sock)s->listen_fd, NULL, 0);
    if (fd == RS_INVALID_SOCK) {
        int err = rs_sock_errno;
        if (err == RS_SOCK_AGAIN) {
            return RS_OK;
        }
#ifndef _WIN32
        if (err == EWOULDBLOCK) {
            return RS_OK;
        }
#endif
        return RS_ERR_IO;
    }
    if (s->nconns >= RS_MAX_CONNS) {
        rs_sock_close(fd);
        return RS_OK;
    }
    c = rs_calloc(1, sizeof(*c));
    if (!c) {
        rs_sock_close(fd);
        return RS_ERR_NOMEM;
    }
    c->srv = s;
    c->fd = (int)fd;
    if (rs_buf_alloc(&c->in, 512) != RS_OK || rs_buf_alloc(&c->out, 512) != RS_OK) {
        rs_conn_free(c);
        return RS_ERR_NOMEM;
    }
    b = rs_ctx_build(s->ctx);
    rs_session_init(&c->session, b);
    c->addr[0] = 0;
    if (s->nconns >= s->cap) {
        int ncap = s->cap ? s->cap * 2 : 16;
        rs_conn **n = rs_realloc(s->conns, (size_t)ncap * sizeof(*n));
        if (!n) {
            rs_conn_free(c);
            return RS_ERR_NOMEM;
        }
        s->conns = n;
        s->cap = ncap;
    }
    s->conns[s->nconns++] = c;
    /* Classic mode waits one poll timeout before this seed. See
     * rs_server_set_classic. An RSC ctx with classic off still speaks first. */
    if (!s->classic && b && b->era == RS_ERA_RSC) {
        rs_handshake hs;
        rs_buf reply;
        uint8_t tmp[8];
        memset(&hs, 0, sizeof(hs));
        hs.link = RS_LINK_LOGIN;
        hs.revision = b->revision;
        c->server_seed = rs_rand_seed();
        rs_buf_wrap(&reply, tmp, sizeof(tmp));
        if (rs_session_handshake_reply(&c->session, &reply, &hs, c->server_seed) == RS_OK &&
            reply.pos > 0) {
            rs_conn_send(c, tmp, reply.pos);
        }
        c->got_hs = 1;
        c->session.link = RS_LINK_LOGIN;
        c->hs = hs;
    }
    if (s->hooks.on_accept) {
        s->hooks.on_accept(c);
    }
    return RS_OK;
}

rs_err rs_server_listen(rs_server *s, const char *host, int port) {
    rs_sock fd;
    if (!s) {
        return RS_ERR_ARG;
    }
    fd = rs_sock_listen(host, port);
    if (fd == RS_INVALID_SOCK) {
        return RS_ERR_IO;
    }
    s->listen_fd = (int)fd;
    s->port = rs_sock_port(fd);
    if (s->port < 0) {
        s->port = port;
    }
    s->running = 1;
    s->next_tick_ms = rs_now_ms() + (uint64_t)s->tick_ms;
    return RS_OK;
}
