#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"

void rs_conn_free(rs_conn *c) {
    if (!c) {
        return;
    }
    if (c->fd >= 0) {
        rs_sock_close((rs_sock)c->fd);
    }
    rs_buf_free(&c->in);
    rs_buf_free(&c->out);
    free(c);
}

void rs_conn_drop(rs_server *s, int idx) {
    rs_conn *c = s->conns[idx];
    if (s->hooks.on_close) {
        s->hooks.on_close(c);
    }
    rs_conn_free(c);
    s->conns[idx] = s->conns[s->nconns - 1];
    s->nconns--;
}

rs_err rs_conn_flush(rs_conn *c) {
    while (c->out.pos > 0) {
        ssize_t n = rs_sock_send((rs_sock)c->fd, c->out.data, c->out.pos);
        if (n < 0) {
            if (rs_sock_errno == RS_SOCK_AGAIN) {
                return RS_OK;
            }
            return RS_ERR_IO;
        }
        if (n == 0) {
            return RS_ERR_IO;
        }
        if ((size_t)n < c->out.pos) {
            memmove(c->out.data, c->out.data + n, c->out.pos - (size_t)n);
        }
        c->out.pos -= (size_t)n;
    }
    return RS_OK;
}

rs_err rs_conn_send(rs_conn *c, const void *data, size_t n) {
    rs_err e;
    if (!c || c->fd < 0) {
        return RS_ERR_ARG;
    }
    e = rs_pdata(&c->out, data, n);
    if (e != RS_OK) {
        return e;
    }
    return rs_conn_flush(c);
}

void rs_conn_close(rs_conn *c) {
    if (c) {
        c->closing = 1;
    }
}

int rs_conn_fd(const rs_conn *c) {
    return c ? c->fd : -1;
}

rs_session *rs_conn_session(rs_conn *c) {
    return c ? &c->session : NULL;
}

const rs_build_info *rs_conn_build(const rs_conn *c) {
    return c ? c->session.build : NULL;
}

rs_server *rs_conn_server(rs_conn *c) {
    return c ? c->srv : NULL;
}

const char *rs_conn_addr(const rs_conn *c) {
    return c ? c->addr : "";
}

rs_link rs_conn_link(const rs_conn *c) {
    return c ? c->session.link : RS_LINK_UNKNOWN;
}

const rs_login_request *rs_conn_login(const rs_conn *c) {
    return c && c->got_login ? &c->login : NULL;
}

void rs_conn_set_user(rs_conn *c, void *user) {
    if (c) {
        c->user = user;
    }
}

void *rs_conn_user(const rs_conn *c) {
    return c ? c->user : NULL;
}

rs_err rs_conn_read(rs_conn *c) {
    uint8_t chunk[4096];
    for (;;) {
        ssize_t n = rs_sock_recv((rs_sock)c->fd, chunk, sizeof(chunk));
        if (n < 0) {
            if (rs_sock_errno == RS_SOCK_AGAIN) {
                break;
            }
            return RS_ERR_IO;
        }
        if (n == 0) {
            return RS_ERR_IO;
        }
        if (c->in.pos + (size_t)n > RS_IN_CAP) {
            return RS_ERR_RANGE;
        }
        {
            rs_err e = rs_pdata(&c->in, chunk, (size_t)n);
            if (e != RS_OK) {
                return e;
            }
        }
    }

    if (!c->got_hs) {
        rs_err e = rs_server_handle_handshake(c);
        if (e == RS_ERR_NEED_MORE) {
            return RS_OK;
        }
        if (e != RS_OK) {
            return e;
        }
    }

    if (c->session.link == RS_LINK_LOGIN && !c->got_login) {
        rs_err e = rs_server_handle_login(c);
        if (e == RS_ERR_NEED_MORE) {
            return RS_OK;
        }
        if (e != RS_OK) {
            return e;
        }
    }

    if (c->session.link == RS_LINK_GAME) {
        return rs_server_handle_game(c);
    }
    if (c->session.link == RS_LINK_ONDEMAND || c->session.link == RS_LINK_JS5) {
        return rs_server_handle_update(c);
    }
    return RS_OK;
}
