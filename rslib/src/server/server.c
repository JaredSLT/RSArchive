#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"

rs_err rs_server_set_cache(rs_server *s, rs_cache *cache) {
    if (!s || !cache) {
        return RS_ERR_ARG;
    }
    if (s->ncaches == 0) {
        s->caches[s->ncaches++] = cache;
        return RS_OK;
    }
    s->caches[0] = cache;
    return RS_OK;
}

rs_err rs_server_add_cache(rs_server *s, rs_cache *cache) {
    int i;
    if (!s || !cache) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < s->ncaches; i++) {
        if (s->caches[i] == cache) {
            return RS_OK;
        }
    }
    if (s->ncaches >= RS_MAX_CACHES) {
        return RS_ERR_RANGE;
    }
    s->caches[s->ncaches++] = cache;
    return RS_OK;
}

rs_cache *rs_server_cache(rs_server *s, const rs_build_info *build) {
    int i;
    if (!s) {
        return NULL;
    }
    if (build) {
        for (i = 0; i < s->ncaches; i++) {
            const rs_build_info *b = rs_cache_build(s->caches[i]);
            if (b && b->revision == build->revision) {
                return s->caches[i];
            }
        }
    }
    return s->ncaches ? s->caches[0] : NULL;
}

void rs_server_set_rsa(rs_server *s, const rs_rsa_key *key) {
    if (!s) {
        return;
    }
    if (!key) {
        s->has_rsa = 0;
        return;
    }
    s->rsa = *key;
    s->has_rsa = 1;
}

void rs_server_set_rsa_for(rs_server *s, int revision, const rs_rsa_key *key) {
    int i, slot;

    if (!s || revision <= 0) {
        return;
    }
    slot = -1;
    for (i = 0; i < RS_RSA_FOR_MAX; i++) {
        if (s->rsa_for[i].present && s->rsa_for[i].revision == revision) {
            slot = i;
            break;
        }
        if (!s->rsa_for[i].present && slot < 0) {
            slot = i;
        }
    }
    if (!key) {
        if (slot >= 0 && s->rsa_for[slot].present && s->rsa_for[slot].revision == revision) {
            s->rsa_for[slot].present = 0;
        }
        return;
    }
    if (slot < 0) {
        slot = 0;
    }
    s->rsa_for[slot].revision = revision;
    s->rsa_for[slot].key = *key;
    s->rsa_for[slot].present = 1;
}

void rs_server_set_classic(rs_server *s, int enable) {
    if (s) {
        s->classic = enable ? 1 : 0;
    }
}

rs_err rs_server_open_archive(rs_server *s, const char *archive_root) {
    rs_cache *cache;
    if (!s || !archive_root || !s->ctx) {
        return RS_ERR_ARG;
    }
    cache = rs_cache_open_archive(s->ctx, archive_root);
    if (!cache) {
        return RS_ERR_IO;
    }
    if (s->nowned >= RS_MAX_CACHES) {
        rs_cache_close(cache);
        return RS_ERR_RANGE;
    }
    s->owned[s->nowned++] = cache;
    return rs_server_set_cache(s, cache);
}

rs_err rs_server_load_archive(rs_server *s, const char *archive_root) {
    size_t n = 0, i;
    const rs_build_info *all;
    if (!s || !archive_root) {
        return RS_ERR_ARG;
    }
    all = rs_build_known(&n);
    for (i = 0; i < n; i++) {
        rs_ctx *ctx;
        rs_cache *cache;
        ctx = rs_ctx_create();
        if (!ctx) {
            return RS_ERR_NOMEM;
        }
        cache = rs_cache_open_build(ctx, archive_root, all[i].id);
        /* ctx is only needed during open; the cache copies build pointer from known table */
        rs_ctx_destroy(ctx);
        if (!cache) {
            continue;
        }
        if (s->nowned >= RS_MAX_CACHES || s->ncaches >= RS_MAX_CACHES) {
            rs_cache_close(cache);
            return RS_ERR_RANGE;
        }
        s->owned[s->nowned++] = cache;
        s->caches[s->ncaches++] = cache;
    }
    return s->ncaches ? RS_OK : RS_ERR_NOT_FOUND;
}

rs_server *rs_server_create(rs_ctx *ctx, const rs_server_hooks *hooks) {
    rs_server *s = rs_calloc(1, sizeof(*s));
    if (!s) {
        return NULL;
    }
    if (rs_sock_init() != RS_OK) {
        free(s);
        return NULL;
    }
    s->ctx = ctx;
    s->listen_fd = -1;
    s->tick_ms = RS_TICK_MS;
    if (hooks) {
        s->hooks = *hooks;
    }
    return s;
}

void rs_server_destroy(rs_server *s) {
    int i;
    if (!s) {
        return;
    }
    for (i = 0; i < s->nconns; i++) {
        if (s->hooks.on_close) {
            s->hooks.on_close(s->conns[i]);
        }
        rs_conn_free(s->conns[i]);
    }
    free(s->conns);
    if (s->listen_fd >= 0) {
        rs_sock_close((rs_sock)s->listen_fd);
    }
    {
        int k;
        for (k = 0; k < s->nowned; k++) {
            rs_cache_close(s->owned[k]);
        }
    }
    free(s);
}

int rs_server_port(const rs_server *s) {
    return s ? s->port : 0;
}

void rs_server_set_tick_ms(rs_server *s, int ms) {
    if (s && ms > 0) {
        s->tick_ms = ms;
    }
}

int rs_server_tick_ms(const rs_server *s) {
    return s ? s->tick_ms : 0;
}

uint64_t rs_server_tick(const rs_server *s) {
    return s ? s->tick : 0;
}

rs_ctx *rs_server_ctx(rs_server *s) {
    return s ? s->ctx : NULL;
}

void rs_server_stop(rs_server *s) {
    if (s) {
        s->running = 0;
    }
}
