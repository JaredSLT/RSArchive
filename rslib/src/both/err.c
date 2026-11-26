#include "internal.h"

const char *rs_strerror(rs_err err) {
    switch (err) {
    case RS_OK:
        return "ok";
    case RS_ERR_NOMEM:
        return "out of memory";
    case RS_ERR_IO:
        return "i/o error";
    case RS_ERR_FORMAT:
        return "invalid format";
    case RS_ERR_UNSUPPORTED:
        return "unsupported";
    case RS_ERR_NOT_FOUND:
        return "not found";
    case RS_ERR_RANGE:
        return "out of range";
    case RS_ERR_CRC:
        return "checksum mismatch";
    case RS_ERR_VERSION:
        return "unknown or unsupported build";
    case RS_ERR_PROTO:
        return "protocol error";
    case RS_ERR_ARG:
        return "invalid argument";
    case RS_ERR_NEED_MORE:
        return "need more data";
    default:
        return "unknown error";
    }
}

const char *rs_version(void) {
    return "0.6.0";
}

void *rs_malloc(size_t n) {
    return malloc(n ? n : 1);
}

void *rs_calloc(size_t n, size_t sz) {
    if (!n || !sz) {
        return calloc(1, 1);
    }
    return calloc(n, sz);
}

void *rs_realloc(void *p, size_t n) {
    return realloc(p, n ? n : 1);
}

rs_err rs_read_file(const char *path, rs_blob *out) {
    FILE *f;
    long sz;
    uint8_t *buf;

    memset(out, 0, sizeof(*out));
    f = fopen(path, "rb");
    if (!f) {
        return RS_ERR_IO;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return RS_ERR_IO;
    }
    sz = ftell(f);
    if (sz < 0) {
        fclose(f);
        return RS_ERR_IO;
    }
    rewind(f);
    buf = rs_malloc((size_t)sz);
    if (!buf) {
        fclose(f);
        return RS_ERR_NOMEM;
    }
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        free(buf);
        fclose(f);
        return RS_ERR_IO;
    }
    fclose(f);
    out->data = buf;
    out->size = (size_t)sz;
    return RS_OK;
}

void rs_blob_free(rs_blob *blob) {
    if (!blob) {
        return;
    }
    free(blob->data);
    blob->data = NULL;
    blob->size = 0;
}
