#ifndef RSLIB_INTERNAL_H
#define RSLIB_INTERNAL_H

#include "rslib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RS_NELEM(a) ((int)(sizeof(a) / sizeof((a)[0])))

static inline int rs_be16(const uint8_t *p) {
    return (p[0] << 8) | p[1];
}

static inline int rs_be24(const uint8_t *p) {
    return (p[0] << 16) | (p[1] << 8) | p[2];
}

static inline uint32_t rs_be32u(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

static inline void rs_put_be16(uint8_t *p, int v) {
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static inline void rs_put_be24(uint8_t *p, int v) {
    p[0] = (uint8_t)(v >> 16);
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)v;
}

static inline void rs_put_be32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

uint64_t rs_now_ms(void);
int rs_set_nonblock(int fd);

void *rs_malloc(size_t n);
void *rs_calloc(size_t n, size_t sz);
void *rs_realloc(void *p, size_t n);

rs_err rs_read_file(const char *path, rs_blob *out);
rs_err rs_bzip2_jagex(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len);
rs_err rs_gzip_inflate(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len);

#endif
