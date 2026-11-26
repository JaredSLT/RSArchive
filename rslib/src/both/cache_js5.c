#include "both/cache_impl.h"

rs_err rs_cache_index(rs_cache *c, int archive, rs_js5_index *out) {
    rs_blob raw;
    rs_err e;
    if (!c || !out) {
        return RS_ERR_ARG;
    }
    if (c->build->cache_kind != RS_CACHE_JS5) {
        return RS_ERR_UNSUPPORTED;
    }
    if (archive >= 0 && archive < c->nstores && c->index_loaded && c->index_loaded[archive]) {
        /* deep copy is heavy; re-parse */
    }
    e = rs_cache_read(c, 255, archive, &raw);
    if (e != RS_OK) {
        return e;
    }
    e = rs_js5_index_parse(raw.data, raw.size, out);
    rs_blob_free(&raw);
    if (e == RS_OK && archive >= 0 && archive < c->nstores && c->indices) {
        if (c->index_loaded[archive]) {
            rs_js5_index_free(&c->indices[archive]);
        }
        /* keep a parsed copy by re-parse into cache slot - skip, caller owns out */
        c->index_loaded[archive] = 0;
    }
    return e;
}

rs_err rs_cache_read_group(rs_cache *c, int archive, int group, rs_blob *out) {
    rs_blob raw;
    rs_err e;
    const rs_xtea_key *key;

    if (!c) {
        return RS_ERR_ARG;
    }
    e = rs_cache_read(c, archive, group, &raw);
    if (e != RS_OK) {
        return e;
    }
    if (archive != 255) {
        rs_js5_index idx;
        if (rs_cache_index(c, archive, &idx) == RS_OK) {
            const rs_js5_group *g = rs_js5_index_group(&idx, group);
            if (g && g->has_whirlpool) {
                uint8_t dig[64];
                rs_whirlpool(raw.data, raw.size, dig);
                if (memcmp(dig, g->whirlpool, 64) != 0) {
                    rs_js5_index_free(&idx);
                    rs_blob_free(&raw);
                    return RS_ERR_FORMAT;
                }
            }
            rs_js5_index_free(&idx);
        }
    }
    key = rs_cache_find_key(c, archive, group);
    if (key && raw.size >= 5) {
        uint32_t k[4];
        memcpy(k, key->key, sizeof(k));
        rs_xtea_decrypt_range(k, raw.data, 5, raw.size);
    }
    e = rs_js5_container_unpack(raw.data, raw.size, out);
    rs_blob_free(&raw);
    return e;
}

rs_err rs_cache_read_file(rs_cache *c, int archive, int group, int file, rs_blob *out) {
    rs_js5_index idx;
    const rs_js5_group *g;
    rs_blob group_data;
    rs_err e;

    e = rs_cache_index(c, archive, &idx);
    if (e != RS_OK) {
        return e;
    }
    g = rs_js5_index_group(&idx, group);
    if (!g) {
        rs_js5_index_free(&idx);
        return RS_ERR_NOT_FOUND;
    }
    e = rs_cache_read_group(c, archive, group, &group_data);
    if (e != RS_OK) {
        rs_js5_index_free(&idx);
        return e;
    }
    e = rs_js5_group_file(group_data.data, group_data.size, g, file, out);
    rs_blob_free(&group_data);
    rs_js5_index_free(&idx);
    return e;
}
