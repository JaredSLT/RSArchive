#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"

rs_err rs_server_poll(rs_server *s, int timeout_ms) {
    rs_pollfd *pf;
    int npoll, i, n, wait;
    uint64_t now;
    rs_err e;

    if (!s || s->listen_fd < 0) {
        return RS_ERR_ARG;
    }
    now = rs_now_ms();
    wait = timeout_ms;
    if (s->tick_ms > 0) {
        int until = (int)(s->next_tick_ms > now ? s->next_tick_ms - now : 0);
        if (wait < 0 || until < wait) {
            wait = until;
        }
    }

    npoll = s->nconns + 1;
    pf = rs_calloc((size_t)npoll, sizeof(*pf));
    if (!pf) {
        return RS_ERR_NOMEM;
    }
    pf[0].fd = s->listen_fd;
    pf[0].events = POLLIN;
    for (i = 0; i < s->nconns; i++) {
        pf[i + 1].fd = s->conns[i]->fd;
        pf[i + 1].events = POLLIN;
        if (s->conns[i]->out.pos > 0) {
            pf[i + 1].events |= POLLOUT;
        }
    }
    n = rs_poll(pf, (unsigned)npoll, wait);
    if (n < 0) {
        free(pf);
        if (rs_sock_errno == RS_SOCK_INTR) {
            return RS_OK;
        }
        return RS_ERR_IO;
    }
    if (pf[0].revents & POLLIN) {
        e = rs_server_accept(s);
        if (e != RS_OK) {
            free(pf);
            return e;
        }
    }
    for (i = s->nconns - 1; i >= 0; i--) {
        rs_conn *c = s->conns[i];
        short rev = 0;
        int j;
        for (j = 1; j < npoll; j++) {
            if (pf[j].fd == c->fd) {
                rev = pf[j].revents;
                break;
            }
        }
        if (rev & (POLLERR | POLLHUP | POLLNVAL)) {
            rs_conn_drop(s, i);
            continue;
        }
        if (rev & POLLIN) {
            e = rs_conn_read(c);
            if (e != RS_OK) {
                rs_conn_drop(s, i);
                continue;
            }
        }
        if (rev & POLLOUT) {
            e = rs_conn_flush(c);
            if (e != RS_OK) {
                rs_conn_drop(s, i);
                continue;
            }
        }
        if (c->closing) {
            rs_conn_drop(s, i);
            continue;
        }
        /* Record one poll timeout with an empty buffer. The handshake
         * handler sends the RSC seed only when classic mode is on. */
        if (n == 0 && !c->got_hs && c->in.pos == 0) {
            c->empty_polls = 1;
            if (s->classic) {
                e = rs_server_handle_handshake(c);
                if (e != RS_OK && e != RS_ERR_NEED_MORE) {
                    rs_conn_drop(s, i);
                }
            }
        }
    }
    free(pf);

    now = rs_now_ms();
    while (s->tick_ms > 0 && now >= s->next_tick_ms) {
        s->tick++;
        s->next_tick_ms += (uint64_t)s->tick_ms;
        if (s->hooks.on_tick) {
            s->hooks.on_tick(s, s->tick);
        }
        if (s->next_tick_ms + (uint64_t)s->tick_ms * 5 < now) {
            s->next_tick_ms = now + (uint64_t)s->tick_ms;
        }
    }
    return RS_OK;
}

rs_err rs_server_run(rs_server *s) {
    rs_err e;
    if (!s) {
        return RS_ERR_ARG;
    }
    s->running = 1;
    while (s->running) {
        e = rs_server_poll(s, -1);
        if (e != RS_OK) {
            return e;
        }
    }
    return RS_OK;
}
