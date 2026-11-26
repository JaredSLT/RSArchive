#include "internal.h"
#include "both/buf_impl.h"

rs_err rs_buf_alloc(rs_buf *b, size_t cap) {
    memset(b, 0, sizeof(*b));
    b->data = rs_malloc(cap ? cap : 1);
    if (!b->data) {
        return RS_ERR_NOMEM;
    }
    b->cap = cap;
    b->owned = 1;
    return RS_OK;
}

void rs_buf_wrap(rs_buf *b, uint8_t *data, size_t len) {
    memset(b, 0, sizeof(*b));
    b->data = data;
    b->cap = len;
}

void rs_buf_free(rs_buf *b) {
    if (!b) {
        return;
    }
    if (b->owned) {
        free(b->data);
    }
    memset(b, 0, sizeof(*b));
}

void rs_buf_reset(rs_buf *b) {
    b->pos = 0;
    b->bit_pos = 0;
}

rs_err rs_buf_ensure(rs_buf *b, size_t extra) {
    size_t need = b->pos + extra;
    size_t ncap;
    uint8_t *p;
    if (need <= b->cap) {
        return RS_OK;
    }
    if (!b->owned) {
        return RS_ERR_RANGE;
    }
    ncap = b->cap ? b->cap : 64;
    while (ncap < need) {
        ncap *= 2;
    }
    p = rs_realloc(b->data, ncap);
    if (!p) {
        return RS_ERR_NOMEM;
    }
    b->data = p;
    b->cap = ncap;
    return RS_OK;
}

size_t rs_buf_remaining(const rs_buf *b) {
    return b->pos <= b->cap ? b->cap - b->pos : 0;
}

rs_err rs_buf_putn(rs_buf *b, const uint8_t *p, size_t n) {
    rs_err e = rs_buf_ensure(b, n);
    if (e != RS_OK) {
        return e;
    }
    memcpy(b->data + b->pos, p, n);
    b->pos += n;
    return RS_OK;
}

int rs_buf_has(const rs_buf *b, size_t n) {
    return b->pos + n <= b->cap;
}
