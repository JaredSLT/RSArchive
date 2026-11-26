#include "both/cache_impl.h"

rs_err rs_cache_get(rs_cache *c, int archive, int id, int child, rs_blob *out) {
    if (!c || !out) {
        return RS_ERR_ARG;
    }
    if (c->build->cache_kind == RS_CACHE_JAG) {
        (void)child;
        return rs_cache_read(c, archive, id, out);
    }
    if (c->build->cache_kind == RS_CACHE_RS2) {
        if (child < 0) {
            return rs_cache_read(c, archive, id, out);
        }
        {
            rs_jag jag;
            rs_err e = rs_cache_read_jag(c, id, &jag);
            if (e != RS_OK) {
                return e;
            }
            if (child >= jag.file_count) {
                rs_jag_close(&jag);
                return RS_ERR_RANGE;
            }
            e = rs_jag_read_hash(&jag, jag.files[child].hash, out);
            rs_jag_close(&jag);
            return e;
        }
    }
    if (child < 0) {
        return rs_cache_read_group(c, archive, id, out);
    }
    return rs_cache_read_file(c, archive, id, child, out);
}

rs_err rs_cache_read(rs_cache *c, int archive, int file, rs_blob *out) {
    if (!c) {
        return RS_ERR_ARG;
    }
    if (c->build->cache_kind == RS_CACHE_JAG) {
        if (archive < 0 || archive >= c->njags) {
            return RS_ERR_RANGE;
        }
        if (file < 0 || file >= c->jags[archive].file_count) {
            return RS_ERR_RANGE;
        }
        return rs_jag_read_hash(&c->jags[archive], c->jags[archive].files[file].hash, out);
    }
    if (archive == 255 && c->master) {
        return rs_store_read(c->master, file, out);
    }
    if (archive < 0 || archive >= RS_MAX_STORES || !c->stores[archive]) {
        return RS_ERR_RANGE;
    }
    return rs_store_read(c->stores[archive], file, out);
}

rs_err rs_cache_write(rs_cache *c, int archive, int file, const uint8_t *data, size_t len) {
    (void)c;
    (void)archive;
    (void)file;
    (void)data;
    (void)len;
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_cache_read_jag(rs_cache *c, int archive_or_index, rs_jag *out) {
    rs_blob raw;
    rs_err e;
    if (!c || !out) {
        return RS_ERR_ARG;
    }
    if (c->build->cache_kind == RS_CACHE_JAG) {
        rs_jag *src;
        if (archive_or_index < 0 || archive_or_index >= c->njags) {
            return RS_ERR_RANGE;
        }
        src = &c->jags[archive_or_index];
        return rs_jag_open_mem(out, src->data, src->size);
    }
    /* RS2 archives live in idx0 */
    e = rs_cache_read(c, 0, archive_or_index, &raw);
    if (e != RS_OK) {
        return e;
    }
    e = rs_jag_open_mem(out, raw.data, raw.size);
    rs_blob_free(&raw);
    return e;
}

rs_err rs_cache_read_named(rs_cache *c, const char *name, rs_blob *out) {
    int i;
    if (!c || !name || !out) {
        return RS_ERR_ARG;
    }
    if (c->build->cache_kind == RS_CACHE_JAG) {
        for (i = 0; i < c->njags; i++) {
            if (rs_jag_read_name(&c->jags[i], name, out) == RS_OK) {
                return RS_OK;
            }
        }
        return RS_ERR_NOT_FOUND;
    }
    if (c->build->cache_kind == RS_CACHE_RS2) {
        for (i = 1; i <= 8; i++) {
            rs_jag jag;
            rs_err e = rs_cache_read_jag(c, i, &jag);
            if (e != RS_OK) {
                continue;
            }
            e = rs_jag_read_name(&jag, name, out);
            rs_jag_close(&jag);
            if (e == RS_OK) {
                return RS_OK;
            }
        }
        return RS_ERR_NOT_FOUND;
    }
    if (c->build->cache_kind == RS_CACHE_JS5) {
        /* configs live in archive 2; look up the group by name hash */
        rs_js5_index idx;
        rs_err e;
        int32_t h = rs_hash_js5(name);
        e = rs_cache_index(c, 2, &idx);
        if (e != RS_OK) {
            return e;
        }
        for (i = 0; i < idx.ngroups; i++) {
            if (idx.groups[i].name_hash == h) {
                e = rs_cache_read_group(c, 2, idx.groups[i].id, out);
                rs_js5_index_free(&idx);
                return e;
            }
        }
        rs_js5_index_free(&idx);
        return RS_ERR_NOT_FOUND;
    }
    return RS_ERR_UNSUPPORTED;
}
