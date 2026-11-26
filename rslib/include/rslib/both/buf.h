#ifndef RSLIB_BUF_H
#define RSLIB_BUF_H

#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rs_blob {
    uint8_t *data;
    size_t size;
} rs_blob;

void rs_blob_free(rs_blob *blob);

typedef struct rs_buf {
    uint8_t *data;
    size_t cap;
    size_t pos;
    size_t bit_pos;
    int owned;
} rs_buf;

rs_err rs_buf_alloc(rs_buf *b, size_t cap);
void rs_buf_wrap(rs_buf *b, uint8_t *data, size_t len);
void rs_buf_free(rs_buf *b);
void rs_buf_reset(rs_buf *b);
rs_err rs_buf_ensure(rs_buf *b, size_t extra);
size_t rs_buf_remaining(const rs_buf *b);

rs_err rs_p1(rs_buf *b, int v);
rs_err rs_p2(rs_buf *b, int v);
rs_err rs_ip2(rs_buf *b, int v);
rs_err rs_p3(rs_buf *b, int v);
rs_err rs_p4(rs_buf *b, int v);
rs_err rs_ip4(rs_buf *b, int v);
rs_err rs_p5(rs_buf *b, uint64_t v);
rs_err rs_p8(rs_buf *b, uint64_t v);
rs_err rs_pdata(rs_buf *b, const void *src, size_t n);
rs_err rs_pjstr(rs_buf *b, const char *s);          /* newline-terminated */
rs_err rs_pjstrnull(rs_buf *b, const char *s);      /* NUL-terminated */
rs_err rs_psize1(rs_buf *b, size_t start);
rs_err rs_psize2(rs_buf *b, size_t start);
rs_err rs_psize4(rs_buf *b, size_t start);
rs_err rs_psmarts(rs_buf *b, int v);

rs_err rs_p1_neg(rs_buf *b, int v);                 /* p1_alt2: -v */
rs_err rs_p1_sub128(rs_buf *b, int v);              /* 128 - v */
rs_err rs_p1_add128(rs_buf *b, int v);              /* v + 128 */
rs_err rs_p2_le(rs_buf *b, int v);
rs_err rs_p2_add128(rs_buf *b, int v);
rs_err rs_p2_le_add128(rs_buf *b, int v);
rs_err rs_p4_alt2(rs_buf *b, int v);
rs_err rs_p4_alt3(rs_buf *b, int v);

int rs_g1(rs_buf *b);
int rs_g1b(rs_buf *b);
int rs_g2(rs_buf *b);
int rs_g2b(rs_buf *b);
int rs_g3(rs_buf *b);
int rs_g4(rs_buf *b);
int32_t rs_g4s(rs_buf *b);
uint64_t rs_g8(rs_buf *b);
int rs_gsmart(rs_buf *b);
int rs_gsmarts(rs_buf *b);
int rs_gsmart2or4(rs_buf *b);
rs_err rs_gdata(rs_buf *b, void *dst, size_t n);
rs_err rs_gjstr(rs_buf *b, char *dst, size_t dst_cap);
rs_err rs_gjstrnull(rs_buf *b, char *dst, size_t dst_cap);

int rs_g1_add128(rs_buf *b);
int rs_g1_neg(rs_buf *b);
int rs_g1_sub128(rs_buf *b);
int rs_g2_le(rs_buf *b);
int rs_g2_add128(rs_buf *b);
int rs_g2_le_add128(rs_buf *b);
int rs_g2b_le(rs_buf *b);
int rs_g4_alt2(rs_buf *b);
int rs_g4_alt3(rs_buf *b);
int rs_ig4(rs_buf *b);

void rs_access_bits(rs_buf *b);
void rs_access_bytes(rs_buf *b);
int rs_gbit(rs_buf *b, int n);
rs_err rs_pbit(rs_buf *b, int n, int v);

#ifdef __cplusplus
}
#endif

#endif
