#define _POSIX_C_SOURCE 200809L

#include "client/client_impl.h"

rs_client *rs_client_create(rs_ctx *ctx, const rs_client_hooks *hooks) {
    rs_client *c = rs_calloc(1, sizeof(*c));
    if (!c) {
        return NULL;
    }
    if (rs_sock_init() != RS_OK) {
        free(c);
        return NULL;
    }
    c->ctx = ctx;
    c->fd = -1;
    c->tick_ms = RS_TICK_MS;
    if (hooks) {
        c->hooks = *hooks;
    }
    rs_session_init_role(&c->session, rs_ctx_build(ctx), RS_ROLE_CLIENT);
    return c;
}

void rs_client_destroy(rs_client *c) {
    if (!c) {
        return;
    }
    if (c->hooks.on_close) {
        c->hooks.on_close(c);
    }
    if (c->fd >= 0) {
        rs_sock_close((rs_sock)c->fd);
    }
    rs_js5_reply_free(&c->js5);
    rs_buf_free(&c->in);
    rs_buf_free(&c->out);
    free(c);
}

void rs_client_set_on_group(rs_client *c, rs_client_on_group fn) {
    if (c) {
        c->on_group = fn;
    }
}

rs_err rs_client_set_cache(rs_client *c, struct rs_cache *cache) {
    if (!c) {
        return RS_ERR_ARG;
    }
    c->cache = cache;
    return RS_OK;
}

void rs_client_set_rsa(rs_client *c, const rs_rsa_key *key) {
    if (!c) {
        return;
    }
    if (!key) {
        c->has_rsa = 0;
        return;
    }
    c->rsa = *key;
    c->has_rsa = 1;
}

void rs_client_set_tick_ms(rs_client *c, int ms) {
    if (c && ms > 0) {
        c->tick_ms = ms;
    }
}

int rs_client_tick_ms(const rs_client *c) {
    return c ? c->tick_ms : 0;
}

uint64_t rs_client_tick(const rs_client *c) {
    return c ? c->tick : 0;
}

rs_ctx *rs_client_ctx(rs_client *c) {
    return c ? c->ctx : NULL;
}

rs_session *rs_client_session(rs_client *c) {
    return c ? &c->session : NULL;
}

const rs_build_info *rs_client_build(const rs_client *c) {
    return c ? c->session.build : NULL;
}

int rs_client_fd(const rs_client *c) {
    return c ? c->fd : -1;
}

void rs_client_stop(rs_client *c) {
    if (c) {
        c->running = 0;
    }
}

void rs_client_close(rs_client *c) {
    if (c) {
        c->closing = 1;
    }
}
