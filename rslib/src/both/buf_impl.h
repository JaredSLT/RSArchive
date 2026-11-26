#ifndef RSLIB_BUF_IMPL_H
#define RSLIB_BUF_IMPL_H

#include "internal.h"

rs_err rs_buf_putn(rs_buf *b, const uint8_t *p, size_t n);
int rs_buf_has(const rs_buf *b, size_t n);

#endif
