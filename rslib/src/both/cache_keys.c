#include "both/cache_impl.h"

rs_err rs_cache_parse_keys(rs_cache *c, const char *json, size_t len) {
    size_t i = 0;
    while (i < len) {
        rs_xtea_key k;
        int got_key = 0, ki = 0;
        memset(&k, 0, sizeof(k));
        k.archive = -1;
        k.group = -1;
        k.mapsquare = -1;
        /* find next object */
        while (i < len && json[i] != '{') {
            i++;
        }
        if (i >= len) {
            break;
        }
        i++;
        while (i < len && json[i] != '}') {
            if (json[i] == '"') {
                char keyname[32];
                int n = 0;
                i++;
                while (i < len && json[i] != '"' && n < 31) {
                    keyname[n++] = json[i++];
                }
                keyname[n] = 0;
                if (i < len && json[i] == '"') {
                    i++;
                }
                while (i < len && (json[i] == ' ' || json[i] == ':' || json[i] == '\n' ||
                                   json[i] == '\r' || json[i] == '\t')) {
                    i++;
                }
                if (strcmp(keyname, "archive") == 0 || strcmp(keyname, "group") == 0 ||
                    strcmp(keyname, "name_hash") == 0 || strcmp(keyname, "mapsquare") == 0) {
                    long v = strtol(json + i, NULL, 10);
                    if (keyname[0] == 'a') {
                        k.archive = (int)v;
                    } else if (keyname[0] == 'g') {
                        k.group = (int)v;
                    } else if (keyname[0] == 'n') {
                        k.name_hash = (int32_t)v;
                    } else {
                        k.mapsquare = (int)v;
                    }
                } else if (strcmp(keyname, "key") == 0) {
                    while (i < len && json[i] != '[') {
                        i++;
                    }
                    if (i < len) {
                        i++;
                    }
                    ki = 0;
                    while (i < len && json[i] != ']' && ki < 4) {
                        char *end;
                        long v;
                        while (i < len && (json[i] == ' ' || json[i] == ',' || json[i] == '\n' ||
                                           json[i] == '\r' || json[i] == '\t')) {
                            i++;
                        }
                        v = strtol(json + i, &end, 10);
                        if (end == json + i) {
                            break;
                        }
                        k.key[ki++] = (uint32_t)v;
                        i = (size_t)(end - json);
                    }
                    got_key = ki == 4;
                }
            } else {
                i++;
            }
        }
        if (got_key && k.group >= 0) {
            rs_xtea_key *nk = rs_realloc(c->keys, (size_t)(c->nkeys + 1) * sizeof(rs_xtea_key));
            if (!nk) {
                return RS_ERR_NOMEM;
            }
            c->keys = nk;
            c->keys[c->nkeys++] = k;
        }
        if (i < len && json[i] == '}') {
            i++;
        }
    }
    return RS_OK;
}

rs_err rs_cache_load_keys(rs_cache *c, const char *keys_json_path) {
    rs_blob b;
    rs_err e;
    if (!c || !keys_json_path) {
        return RS_ERR_ARG;
    }
    e = rs_read_file(keys_json_path, &b);
    if (e != RS_OK) {
        return e;
    }
    e = rs_cache_parse_keys(c, (char *)b.data, b.size);
    rs_blob_free(&b);
    return e;
}

const rs_xtea_key *rs_cache_find_key(const rs_cache *c, int archive, int group) {
    int i;
    if (!c) {
        return NULL;
    }
    for (i = 0; i < c->nkeys; i++) {
        if (c->keys[i].group == group && (archive < 0 || c->keys[i].archive == archive ||
                                          c->keys[i].archive < 0)) {
            return &c->keys[i];
        }
    }
    return NULL;
}

const rs_xtea_key *rs_cache_find_key_mapsquare(const rs_cache *c, int mapsquare) {
    int i;
    if (!c) {
        return NULL;
    }
    for (i = 0; i < c->nkeys; i++) {
        if (c->keys[i].mapsquare == mapsquare) {
            return &c->keys[i];
        }
    }
    return rs_cache_find_key(c, -1, mapsquare);
}
