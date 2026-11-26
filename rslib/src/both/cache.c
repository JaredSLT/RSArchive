#include "both/cache_impl.h"

int rs_cache_path_join(char *dst, size_t cap, const char *dir, const char *name) {
    int n = snprintf(dst, cap, "%s/%s", dir, name);
    return n < 0 || (size_t)n >= cap ? -1 : 0;
}

int rs_cache_file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

rs_err rs_cache_load_jags(rs_cache *c) {
    DIR *d = opendir(c->dir);
    struct dirent *de;
    if (!d) {
        return RS_ERR_IO;
    }
    while ((de = readdir(d)) != NULL) {
        size_t n = strlen(de->d_name);
        char path[1024];
        rs_jag jag;
        rs_err e;
        rs_jag *list;
        char **names;
        if (n < 5 || strcmp(de->d_name + n - 4, ".jag") != 0) {
            continue;
        }
        if (rs_cache_path_join(path, sizeof(path), c->dir, de->d_name) != 0) {
            continue;
        }
        e = rs_jag_open_file(&jag, path);
        if (e != RS_OK) {
            closedir(d);
            return e;
        }
        list = rs_realloc(c->jags, (size_t)(c->njags + 1) * sizeof(rs_jag));
        names = rs_realloc(c->jag_names, (size_t)(c->njags + 1) * sizeof(char *));
        if (!list || !names) {
            rs_jag_close(&jag);
            closedir(d);
            return RS_ERR_NOMEM;
        }
        c->jags = list;
        c->jag_names = names;
        c->jags[c->njags] = jag;
        c->jag_names[c->njags] = strcpy(rs_malloc(n + 1), de->d_name);
        c->njags++;
    }
    closedir(d);
    return RS_OK;
}

rs_cache *rs_cache_open(rs_ctx *ctx, const char *dir) {
    rs_cache *c;
    const rs_build_info *b;
    char dat[1024], idx[1024], keys[1024];
    int i;

    if (!ctx || !dir) {
        return NULL;
    }
    b = rs_ctx_build(ctx);
    if (!b) {
        return NULL;
    }
    c = rs_calloc(1, sizeof(*c));
    if (!c) {
        return NULL;
    }
    c->build = b;
    c->dir = strcpy(rs_malloc(strlen(dir) + 1), dir);
    if (!c->dir) {
        free(c);
        return NULL;
    }

    if (b->cache_kind == RS_CACHE_JAG) {
        if (rs_cache_load_jags(c) != RS_OK) {
            rs_cache_close(c);
            return NULL;
        }
        return c;
    }

    if (b->cache_kind == RS_CACHE_RS2) {
        if (rs_cache_path_join(dat, sizeof(dat), dir, "main_file_cache.dat") != 0) {
            rs_cache_close(c);
            return NULL;
        }
        c->nstores = b->store_archives > 0 ? b->store_archives : 5;
        for (i = 0; i < c->nstores; i++) {
            char name[64];
            snprintf(name, sizeof(name), "main_file_cache.idx%d", i);
            if (rs_cache_path_join(idx, sizeof(idx), dir, name) != 0) {
                rs_cache_close(c);
                return NULL;
            }
            if (rs_store_open(&c->stores[i], dat, idx, i + 1, 0) != RS_OK) {
                rs_cache_close(c);
                return NULL;
            }
        }
        return c;
    }

    if (b->cache_kind == RS_CACHE_JS5) {
        if (rs_cache_path_join(dat, sizeof(dat), dir, "main_file_cache.dat2") != 0) {
            rs_cache_close(c);
            return NULL;
        }
        c->nstores = b->store_archives > 0 ? b->store_archives : 16;
        for (i = 0; i < c->nstores; i++) {
            char name[64];
            snprintf(name, sizeof(name), "main_file_cache.idx%d", i);
            if (rs_cache_path_join(idx, sizeof(idx), dir, name) != 0) {
                continue;
            }
            if (!rs_cache_file_exists(idx)) {
                continue;
            }
            if (rs_store_open(&c->stores[i], dat, idx, i, b->large_file_ids) != RS_OK) {
                rs_cache_close(c);
                return NULL;
            }
        }
        if (rs_cache_path_join(idx, sizeof(idx), dir, "main_file_cache.idx255") == 0 && rs_cache_file_exists(idx)) {
            if (rs_store_open(&c->master, dat, idx, 255, b->large_file_ids) != RS_OK) {
                rs_cache_close(c);
                return NULL;
            }
        }
        c->indices = rs_calloc((size_t)c->nstores, sizeof(rs_js5_index));
        c->index_loaded = rs_calloc((size_t)c->nstores, sizeof(int));
        if (rs_cache_path_join(keys, sizeof(keys), dir, "keys.json") == 0 && rs_cache_file_exists(keys)) {
            rs_cache_load_keys(c, keys);
        }
        return c;
    }

    rs_cache_close(c);
    return NULL;
}

void rs_cache_close(rs_cache *c) {
    int i;
    if (!c) {
        return;
    }
    for (i = 0; i < RS_MAX_STORES; i++) {
        rs_store_close(c->stores[i]);
    }
    rs_store_close(c->master);
    if (c->jags) {
        for (i = 0; i < c->njags; i++) {
            rs_jag_close(&c->jags[i]);
            free(c->jag_names[i]);
        }
        free(c->jags);
        free(c->jag_names);
    }
    if (c->indices && c->index_loaded) {
        for (i = 0; i < c->nstores; i++) {
            if (c->index_loaded[i]) {
                rs_js5_index_free(&c->indices[i]);
            }
        }
    }
    free(c->indices);
    free(c->index_loaded);
    free(c->keys);
    free(c->dir);
    free(c);
}

const rs_build_info *rs_cache_build(const rs_cache *c) {
    return c ? c->build : NULL;
}

int rs_cache_archive_count(const rs_cache *c) {
    if (!c) {
        return 0;
    }
    if (c->build->cache_kind == RS_CACHE_JAG) {
        return c->njags;
    }
    return c->nstores;
}

rs_cache *rs_cache_open_archive(rs_ctx *ctx, const char *archive_root) {
    const rs_build_info *b;
    char path[1024];
    char rel[160];
    if (!ctx || !archive_root) {
        return NULL;
    }
    b = rs_ctx_build(ctx);
    if (!b || !b->id) {
        return NULL;
    }
    if (rs_build_archive_relpath(b, rel, sizeof(rel)) != 0) {
        return NULL;
    }
    if (snprintf(path, sizeof(path), "%s/%s/cache", archive_root, rel) >= (int)sizeof(path)) {
        return NULL;
    }
    return rs_cache_open(ctx, path);
}

rs_cache *rs_cache_open_build(rs_ctx *ctx, const char *archive_root, const char *id_or_rev) {
    if (!ctx || rs_ctx_set_build_id(ctx, id_or_rev) != RS_OK) {
        return NULL;
    }
    return rs_cache_open_archive(ctx, archive_root);
}
