#include "internal.h"

static rs_err jag_parse(rs_jag *jag, uint8_t *owned, size_t len) {
    rs_buf b;
    int unpacked, packed, i, hdr;
    uint8_t *body = owned;

    memset(jag, 0, sizeof(*jag));
    if (len < 6) {
        free(owned);
        return RS_ERR_FORMAT;
    }
    rs_buf_wrap(&b, owned, len);
    unpacked = rs_g3(&b);
    packed = rs_g3(&b);
    if (unpacked != packed) {
        uint8_t *plain = rs_malloc((size_t)unpacked);
        rs_err e;
        if (!plain) {
            free(owned);
            return RS_ERR_NOMEM;
        }
        e = rs_bzip2_jagex(owned + 6, (size_t)packed, plain, (size_t)unpacked);
        free(owned);
        if (e != RS_OK) {
            free(plain);
            return e;
        }
        body = plain;
        len = (size_t)unpacked;
        jag->unpacked_whole = 1;
        rs_buf_wrap(&b, body, len);
    }
    if (rs_buf_remaining(&b) < 2) {
        free(body);
        return RS_ERR_FORMAT;
    }
    jag->file_count = rs_g2(&b);
    if (jag->file_count < 0 || (size_t)jag->file_count * 10 > rs_buf_remaining(&b)) {
        free(body);
        return RS_ERR_FORMAT;
    }
    jag->files = rs_calloc((size_t)jag->file_count, sizeof(rs_jag_entry));
    if (!jag->files) {
        free(body);
        return RS_ERR_NOMEM;
    }
    hdr = (int)b.pos + jag->file_count * 10;
    for (i = 0; i < jag->file_count; i++) {
        jag->files[i].hash = rs_g4s(&b);
        jag->files[i].unpacked = rs_g3(&b);
        jag->files[i].packed = rs_g3(&b);
        jag->files[i].offset = hdr;
        hdr += jag->files[i].packed;
    }
    jag->data = body;
    jag->size = len;
    return RS_OK;
}

rs_err rs_jag_open_mem(rs_jag *jag, const uint8_t *bytes, size_t len) {
    uint8_t *copy;
    if (!jag || !bytes) {
        return RS_ERR_ARG;
    }
    memset(jag, 0, sizeof(*jag));
    copy = rs_malloc(len ? len : 1);
    if (!copy) {
        return RS_ERR_NOMEM;
    }
    memcpy(copy, bytes, len);
    return jag_parse(jag, copy, len);
}

rs_err rs_jag_open_file(rs_jag *jag, const char *path) {
    rs_blob blob;
    rs_err e;
    if (!jag || !path) {
        return RS_ERR_ARG;
    }
    memset(jag, 0, sizeof(*jag));
    e = rs_read_file(path, &blob);
    if (e != RS_OK) {
        return e;
    }
    return jag_parse(jag, blob.data, blob.size);
}

void rs_jag_close(rs_jag *jag) {
    if (!jag) {
        return;
    }
    free(jag->data);
    free(jag->files);
    memset(jag, 0, sizeof(*jag));
}

rs_err rs_jag_read_hash(const rs_jag *jag, int32_t hash, rs_blob *out) {
    int i;
    memset(out, 0, sizeof(*out));
    if (!jag) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < jag->file_count; i++) {
        const rs_jag_entry *e = &jag->files[i];
        if (e->hash != hash) {
            continue;
        }
        out->data = rs_malloc((size_t)e->unpacked ? (size_t)e->unpacked : 1);
        if (!out->data) {
            return RS_ERR_NOMEM;
        }
        out->size = (size_t)e->unpacked;
        if (jag->unpacked_whole) {
            if (e->offset < 0 || (size_t)e->offset + (size_t)e->unpacked > jag->size) {
                rs_blob_free(out);
                return RS_ERR_FORMAT;
            }
            memcpy(out->data, jag->data + e->offset, (size_t)e->unpacked);
            return RS_OK;
        }
        return rs_bzip2_jagex(jag->data + e->offset, (size_t)e->packed, out->data,
                              (size_t)e->unpacked);
    }
    return RS_ERR_NOT_FOUND;
}

rs_err rs_jag_read_name(const rs_jag *jag, const char *name, rs_blob *out) {
    return rs_jag_read_hash(jag, rs_hash_jag(name), out);
}
