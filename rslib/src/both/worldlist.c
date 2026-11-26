#include "internal.h"

rs_err rs_worldlist_write_request(rs_buf *dst, uint32_t checksum) {
    rs_err e = rs_p1(dst, 255);
    if (e) {
        return e;
    }
    return rs_p4(dst, (int)checksum);
}

rs_err rs_worldlist_parse_request(const uint8_t *data, size_t len, uint32_t *checksum,
                                  size_t *consumed) {
    if (!data || len < 5) {
        return RS_ERR_NEED_MORE;
    }
    if ((data[0] & 0xff) != 255) {
        return RS_ERR_PROTO;
    }
    if (checksum) {
        *checksum = rs_be32u(data + 1);
    }
    if (consumed) {
        *consumed = 5;
    }
    return RS_OK;
}

rs_err rs_worldlist_encode(rs_buf *dst, const rs_worldlist *list) {
    int i;
    rs_err e;
    if (!dst || !list) {
        return RS_ERR_ARG;
    }
    e = rs_psmarts(dst, list->ncountries);
    if (e) {
        return e;
    }
    for (i = 0; i < list->ncountries; i++) {
        e = rs_psmarts(dst, list->countries[i].flag);
        if (e) {
            return e;
        }
        e = rs_pjstrnull(dst, list->countries[i].name);
        if (e) {
            return e;
        }
    }
    e = rs_psmarts(dst, list->min_id);
    if (e) {
        return e;
    }
    e = rs_psmarts(dst, list->max_id);
    if (e) {
        return e;
    }
    e = rs_psmarts(dst, list->nworlds);
    if (e) {
        return e;
    }
    for (i = 0; i < list->nworlds; i++) {
        e = rs_psmarts(dst, list->worlds[i].id - list->min_id);
        if (e) {
            return e;
        }
        e = rs_p1(dst, list->worlds[i].country);
        if (e) {
            return e;
        }
        e = rs_p4(dst, (int)list->worlds[i].flags);
        if (e) {
            return e;
        }
        e = rs_pjstrnull(dst, list->worlds[i].activity);
        if (e) {
            return e;
        }
        e = rs_pjstrnull(dst, list->worlds[i].hostname);
        if (e) {
            return e;
        }
        e = rs_p2(dst, list->worlds[i].population);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_worldlist_decode(rs_buf *src, rs_worldlist *list) {
    int i;
    if (!src || !list) {
        return RS_ERR_ARG;
    }
    memset(list, 0, sizeof(*list));
    list->ncountries = rs_gsmarts(src);
    if (list->ncountries < 0 || list->ncountries > RS_COUNTRY_MAX) {
        return RS_ERR_FORMAT;
    }
    for (i = 0; i < list->ncountries; i++) {
        list->countries[i].flag = rs_gsmarts(src);
        if (rs_gjstrnull(src, list->countries[i].name, sizeof(list->countries[i].name)) != RS_OK) {
            return RS_ERR_FORMAT;
        }
    }
    list->min_id = rs_gsmarts(src);
    list->max_id = rs_gsmarts(src);
    list->nworlds = rs_gsmarts(src);
    if (list->nworlds < 0 || list->nworlds > RS_WORLD_MAX) {
        return RS_ERR_FORMAT;
    }
    for (i = 0; i < list->nworlds; i++) {
        int off = rs_gsmarts(src);
        list->worlds[i].id = list->min_id + off;
        list->worlds[i].country = rs_g1(src);
        list->worlds[i].flags = (uint32_t)rs_g4(src);
        if (rs_gjstrnull(src, list->worlds[i].activity, sizeof(list->worlds[i].activity)) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        if (rs_gjstrnull(src, list->worlds[i].hostname, sizeof(list->worlds[i].hostname)) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        list->worlds[i].population = rs_g2(src);
    }
    return RS_OK;
}

rs_err rs_worldlist_write_reply(rs_buf *dst, const rs_worldlist *list) {
    rs_buf inner;
    rs_err e;
    if (rs_buf_alloc(&inner, 256) != RS_OK) {
        return RS_ERR_NOMEM;
    }
    e = rs_worldlist_encode(&inner, list);
    if (e == RS_OK) {
        e = rs_p1(dst, 0);
        if (e == RS_OK) {
            e = rs_p2(dst, (int)inner.pos);
        }
        if (e == RS_OK) {
            e = rs_pdata(dst, inner.data, inner.pos);
        }
    }
    rs_buf_free(&inner);
    return e;
}
