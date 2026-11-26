#ifndef RSLIB_ZONEBUF_H
#define RSLIB_ZONEBUF_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rs_zonebuf rs_zonebuf;

rs_zonebuf *rs_zonebuf_create(int revision);
void rs_zonebuf_destroy(rs_zonebuf *z);
rs_err rs_zonebuf_begin(rs_zonebuf *z, int base_x, int base_z);
/* opcode is the raw zone opcode, not an ISAAC game opcode.
   body is what the client reads after that opcode. */
rs_err rs_zonebuf_append(rs_zonebuf *z, int opcode, const uint8_t *body, size_t n);
/* Zone-partial payload only. The outer game opcode is not written. */
rs_err rs_zonebuf_finish(rs_zonebuf *z, rs_buf *dst);

#ifdef __cplusplus
}
#endif

#endif
