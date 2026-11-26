#define _POSIX_C_SOURCE 200809L

#include "client/client_impl.h"

static void client_js5_group(void *user, int archive, int group, const uint8_t *container, size_t n) {
    rs_client *c = user;
    if (c && c->on_group) {
        c->on_group(c, archive, group, container, n);
    }
}

rs_err rs_client_handle_in(rs_client *c) {
    if (!c->got_hs_reply && c->session.link != RS_LINK_UNKNOWN &&
        c->session.link != RS_LINK_GAME) {
        rs_buf src;
        int status = 0;
        uint64_t seed = 0;
        rs_err e;
        rs_buf_wrap(&src, c->in.data, c->in.pos);
        e = rs_session_handshake_reply_parse(&c->session, &src, c->session.link, &status, &seed);
        if (e == RS_ERR_NEED_MORE) {
            return RS_OK;
        }
        if (e != RS_OK) {
            return e;
        }
        memmove(c->in.data, c->in.data + src.pos, c->in.pos - src.pos);
        c->in.pos -= src.pos;
        c->got_hs_reply = 1;
        c->server_seed = seed;
        c->login_status = status;
        if (c->hooks.on_handshake) {
            c->hooks.on_handshake(c, &c->hs);
        }
    }

    if (c->session.link == RS_LINK_LOGIN && c->got_hs_reply && !c->got_login_reply) {
        rs_buf src;
        int status = 0, rights = 0, flagged = 0;
        rs_err e;
        if (c->in.pos == 0) {
            return RS_OK;
        }
        rs_buf_wrap(&src, c->in.data, c->in.pos);
        e = rs_session_login_success_parse(&c->session, &src, &status, &rights, &flagged);
        if (e == RS_ERR_NEED_MORE) {
            return RS_OK;
        }
        if (e != RS_OK) {
            return e;
        }
        memmove(c->in.data, c->in.data + src.pos, c->in.pos - src.pos);
        c->in.pos -= src.pos;
        c->got_login_reply = 1;
        c->login_status = status;
        c->rights = rights;
        c->flagged = flagged;
        if (status == RS_LOGIN_OK || status == RS_LOGIN_RECONNECT_OK ||
            (c->session.build && c->session.build->era == RS_ERA_RSC && status == 0)) {
            rs_session_seed_isaac(&c->session, c->isaac_seed);
            c->session.link = RS_LINK_GAME;
        }
        if (c->hooks.on_login) {
            c->hooks.on_login(c, status, rights, flagged);
        }
    }

    if (c->session.link == RS_LINK_GAME) {
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
            if (c->hooks.on_packet) {
                c->hooks.on_packet(c, id, opcode, &payload);
            }
            memmove(c->in.data, c->in.data + used, c->in.pos - used);
            c->in.pos -= used;
        }
    }

    /* RS_LINK_ONDEMAND stays on the raw on_update hook. Each reply is an
     * independent chunk (archive, file, size, chunk index, <=500 bytes)
     * from rs_ondemand_write_chunk, not a JS5 container, so it is not fed
     * through rs_js5_reply_feed. */
    if (c->session.link == RS_LINK_ONDEMAND) {
        if (c->in.pos > 0 && c->hooks.on_update) {
            c->hooks.on_update(c, c->in.data, c->in.pos);
            c->in.pos = 0;
        }
    }
    if (c->session.link == RS_LINK_JS5 && c->in.pos > 0) {
        int rev = c->session.build ? c->session.build->revision : 0;
        rs_err e;
        c->js5.encrypt = c->session.js5_encrypt;
        c->js5.key = c->session.js5_key;
        e = rs_js5_reply_feed(&c->js5, rev, c->in.data, c->in.pos, client_js5_group, c);
        c->in.pos = 0;
        if (e != RS_OK) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_client_poll(rs_client *c, int timeout_ms) {
    rs_pollfd pf;
    int n, wait;
    uint64_t now;
    rs_err e;

    if (!c || c->fd < 0) {
        return RS_ERR_ARG;
    }
    now = rs_now_ms();
    wait = timeout_ms;
    if (c->tick_ms > 0) {
        int until = (int)(c->next_tick_ms > now ? c->next_tick_ms - now : 0);
        if (wait < 0 || until < wait) {
            wait = until;
        }
    }
    pf.fd = (rs_sock)c->fd;
    pf.events = POLLIN;
    if (c->out.pos > 0) {
        pf.events |= POLLOUT;
    }
    n = rs_poll(&pf, 1, wait);
    if (n < 0) {
        if (rs_sock_errno == RS_SOCK_INTR) {
            return RS_OK;
        }
        return RS_ERR_IO;
    }
    if (pf.revents & (POLLERR | POLLHUP | POLLNVAL)) {
        return RS_ERR_IO;
    }
    if (pf.revents & POLLIN) {
        uint8_t chunk[4096];
        for (;;) {
            ssize_t got = rs_sock_recv((rs_sock)c->fd, chunk, sizeof(chunk));
            if (got < 0) {
                if (rs_sock_errno == RS_SOCK_AGAIN) {
                    break;
                }
                return RS_ERR_IO;
            }
            if (got == 0) {
                return RS_ERR_IO;
            }
            if (c->in.pos + (size_t)got > RS_IN_CAP) {
                return RS_ERR_RANGE;
            }
            e = rs_pdata(&c->in, chunk, (size_t)got);
            if (e != RS_OK) {
                return e;
            }
        }
        e = rs_client_handle_in(c);
        if (e != RS_OK) {
            return e;
        }
    }
    if (pf.revents & POLLOUT) {
        e = rs_client_flush(c);
        if (e != RS_OK) {
            return e;
        }
    }
    if (c->closing) {
        return RS_ERR_IO;
    }
    now = rs_now_ms();
    while (c->tick_ms > 0 && now >= c->next_tick_ms) {
        c->tick++;
        c->next_tick_ms += (uint64_t)c->tick_ms;
        if (c->hooks.on_tick) {
            c->hooks.on_tick(c, c->tick);
        }
        if (c->next_tick_ms + (uint64_t)c->tick_ms * 5 < now) {
            c->next_tick_ms = now + (uint64_t)c->tick_ms;
        }
    }
    return RS_OK;
}

rs_err rs_client_run(rs_client *c) {
    rs_err e;
    if (!c) {
        return RS_ERR_ARG;
    }
    c->running = 1;
    while (c->running) {
        e = rs_client_poll(c, -1);
        if (e != RS_OK) {
            return e;
        }
    }
    return RS_OK;
}
