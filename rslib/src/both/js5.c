#include "internal.h"

rs_err rs_js5_container_unpack(const uint8_t *src, size_t len, rs_blob *out) {
    rs_buf b;
    int type, clen, ulen;
    rs_err e;

    memset(out, 0, sizeof(*out));
    if (!src || len < 5) {
        return RS_ERR_FORMAT;
    }
    rs_buf_wrap(&b, (uint8_t *)src, len);
    type = rs_g1(&b);
    clen = rs_g4(&b);
    if (clen < 0 || b.pos + (size_t)clen > len) {
        /* some stores include a 2-byte version trailer after the payload */
        if (clen < 0) {
            return RS_ERR_FORMAT;
        }
    }
    if (type == RS_JS5_NONE) {
        if (b.pos + (size_t)clen > len) {
            return RS_ERR_FORMAT;
        }
        out->data = rs_malloc((size_t)clen ? (size_t)clen : 1);
        if (!out->data) {
            return RS_ERR_NOMEM;
        }
        memcpy(out->data, src + b.pos, (size_t)clen);
        out->size = (size_t)clen;
        return RS_OK;
    }
    ulen = rs_g4(&b);
    if (ulen < 0 || ulen > 32 * 1024 * 1024) {
        return RS_ERR_FORMAT;
    }
    if (b.pos + (size_t)clen > len) {
        return RS_ERR_FORMAT;
    }
    out->data = rs_malloc((size_t)ulen ? (size_t)ulen : 1);
    if (!out->data) {
        return RS_ERR_NOMEM;
    }
    out->size = (size_t)ulen;
    if (type == RS_JS5_BZIP2) {
        e = rs_bzip2_jagex(src + b.pos, (size_t)clen, out->data, (size_t)ulen);
    } else if (type == RS_JS5_GZIP) {
        e = rs_gzip_inflate(src + b.pos, (size_t)clen, out->data, (size_t)ulen);
    } else {
        e = RS_ERR_UNSUPPORTED;
    }
    if (e != RS_OK) {
        rs_blob_free(out);
    }
    return e;
}

static int read_id(rs_buf *b, int fmt) {
    return fmt >= 7 ? rs_gsmart2or4(b) : rs_g2(b);
}

rs_err rs_js5_index_parse(const uint8_t *src, size_t len, rs_js5_index *idx) {
    rs_blob plain;
    rs_buf b;
    rs_err e;
    int i, fmt, flags, named, whirl, ngroups, acc, max_id;
    uint8_t *p;
    size_t plen;

    memset(idx, 0, sizeof(*idx));
    if (!src) {
        return RS_ERR_ARG;
    }
    idx->crc = rs_crc32(src, len);
    e = rs_js5_container_unpack(src, len, &plain);
    if (e != RS_OK) {
        return e;
    }
    p = plain.data;
    plen = plain.size;
    rs_buf_wrap(&b, p, plen);
    fmt = rs_g1(&b);
    if (fmt < 5 || fmt > 7) {
        rs_blob_free(&plain);
        return RS_ERR_VERSION;
    }
    idx->format = fmt;
    if (fmt >= 6) {
        idx->version = rs_g4s(&b);
    }
    flags = rs_g1(&b);
    idx->flags = flags;
    named = flags & 1;
    whirl = flags & 2;
    ngroups = read_id(&b, fmt);
    if (ngroups < 0 || ngroups > 65536) {
        rs_blob_free(&plain);
        return RS_ERR_FORMAT;
    }
    idx->ngroups = ngroups;
    idx->groups = rs_calloc((size_t)ngroups, sizeof(rs_js5_group));
    if (!idx->groups) {
        rs_blob_free(&plain);
        return RS_ERR_NOMEM;
    }
    acc = 0;
    max_id = -1;
    for (i = 0; i < ngroups; i++) {
        acc += read_id(&b, fmt);
        idx->groups[i].id = acc;
        if (acc > max_id) {
            max_id = acc;
        }
    }
    idx->capacity = max_id + 1;
    if (named) {
        for (i = 0; i < ngroups; i++) {
            idx->groups[i].name_hash = rs_g4s(&b);
        }
    } else {
        for (i = 0; i < ngroups; i++) {
            idx->groups[i].name_hash = -1;
        }
    }
    for (i = 0; i < ngroups; i++) {
        idx->groups[i].crc = (uint32_t)rs_g4(&b);
    }
    if (whirl) {
        for (i = 0; i < ngroups; i++) {
            if (rs_gdata(&b, idx->groups[i].whirlpool, 64) != RS_OK) {
                rs_blob_free(&plain);
                rs_js5_index_free(idx);
                return RS_ERR_FORMAT;
            }
            idx->groups[i].has_whirlpool = 1;
        }
    }
    for (i = 0; i < ngroups; i++) {
        idx->groups[i].version = rs_g4s(&b);
    }
    for (i = 0; i < ngroups; i++) {
        idx->groups[i].nfiles = read_id(&b, fmt);
    }
    for (i = 0; i < ngroups; i++) {
        rs_js5_group *g = &idx->groups[i];
        int f, facc = 0, fmax = -1;
        g->file_ids = rs_calloc((size_t)g->nfiles, sizeof(int));
        if (!g->file_ids) {
            rs_blob_free(&plain);
            rs_js5_index_free(idx);
            return RS_ERR_NOMEM;
        }
        for (f = 0; f < g->nfiles; f++) {
            facc += read_id(&b, fmt);
            g->file_ids[f] = facc;
            if (facc > fmax) {
                fmax = facc;
            }
        }
        g->capacity = fmax + 1;
        if (g->capacity == g->nfiles) {
            free(g->file_ids);
            g->file_ids = NULL;
        }
    }
    if (named) {
        for (i = 0; i < ngroups; i++) {
            rs_js5_group *g = &idx->groups[i];
            int f, n = g->file_ids ? g->nfiles : g->nfiles;
            g->file_names = rs_malloc((size_t)g->capacity * sizeof(int32_t));
            if (!g->file_names) {
                rs_blob_free(&plain);
                rs_js5_index_free(idx);
                return RS_ERR_NOMEM;
            }
            for (f = 0; f < g->capacity; f++) {
                g->file_names[f] = -1;
            }
            for (f = 0; f < n; f++) {
                int fid = g->file_ids ? g->file_ids[f] : f;
                g->file_names[fid] = rs_g4s(&b);
            }
        }
    }
    rs_blob_free(&plain);
    return RS_OK;
}

void rs_js5_index_free(rs_js5_index *idx) {
    int i;
    if (!idx) {
        return;
    }
    if (idx->groups) {
        for (i = 0; i < idx->ngroups; i++) {
            free(idx->groups[i].file_ids);
            free(idx->groups[i].file_names);
        }
        free(idx->groups);
    }
    memset(idx, 0, sizeof(*idx));
}

const rs_js5_group *rs_js5_index_group(const rs_js5_index *idx, int group_id) {
    int i;
    if (!idx) {
        return NULL;
    }
    for (i = 0; i < idx->ngroups; i++) {
        if (idx->groups[i].id == group_id) {
            return &idx->groups[i];
        }
    }
    return NULL;
}

rs_err rs_js5_group_file(const uint8_t *unpacked, size_t len, const rs_js5_group *g,
                         int file_id, rs_blob *out) {
    int nfiles, stripes, table, i, s, want, off;
    const uint8_t *p;

    memset(out, 0, sizeof(*out));
    if (!unpacked || !g) {
        return RS_ERR_ARG;
    }
    nfiles = g->nfiles;
    if (nfiles <= 1) {
        int fid = g->file_ids ? g->file_ids[0] : 0;
        if (file_id != fid && !(g->file_ids == NULL && file_id == 0)) {
            return RS_ERR_NOT_FOUND;
        }
        out->data = rs_malloc(len ? len : 1);
        if (!out->data) {
            return RS_ERR_NOMEM;
        }
        memcpy(out->data, unpacked, len);
        out->size = len;
        return RS_OK;
    }
    if (len < 1) {
        return RS_ERR_FORMAT;
    }
    stripes = unpacked[len - 1] & 0xff;
    table = (int)len - 1 - nfiles * stripes * 4;
    if (table < 0) {
        return RS_ERR_FORMAT;
    }
    want = -1;
    for (i = 0; i < nfiles; i++) {
        int fid = g->file_ids ? g->file_ids[i] : i;
        if (fid == file_id) {
            want = i;
            break;
        }
    }
    if (want < 0) {
        return RS_ERR_NOT_FOUND;
    }
    {
        int size = 0;
        off = 0;
        p = unpacked + table;
        for (s = 0; s < stripes; s++) {
            int delta = 0;
            for (i = 0; i < nfiles; i++) {
                delta += (int)rs_be32u(p);
                p += 4;
                if (i == want) {
                    size += delta;
                }
            }
        }
        if (size < 0 || (size_t)size > len) {
            return RS_ERR_FORMAT;
        }
        out->data = rs_malloc((size_t)size ? (size_t)size : 1);
        if (!out->data) {
            return RS_ERR_NOMEM;
        }
        out->size = (size_t)size;
        p = unpacked + table;
        off = 0;
        {
            int woff = 0;
            int src = 0;
            for (s = 0; s < stripes; s++) {
                int delta = 0;
                for (i = 0; i < nfiles; i++) {
                    delta += (int)rs_be32u(p);
                    p += 4;
                    if (i == want) {
                        memcpy(out->data + woff, unpacked + src, (size_t)delta);
                        woff += delta;
                    }
                    src += delta;
                }
                (void)off;
            }
        }
    }
    return RS_OK;
}
