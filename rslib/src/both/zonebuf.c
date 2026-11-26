#include "internal.h"
#include "rslib/both/zonebuf.h"

#include <string.h>

/*
 * Enclosed zone payload, matching the client loop that reads a base then
 * raw opcodes until the packet length is consumed.
 *
 * 474 JavaMouseWheel opcode 163: g1 x, g1_sub128 z, then g1 + ShapedTile.opShapedTile20.
 * 530 SecondaryNode opcode 230: g1_add128 z, g1_sub128 x, then g1 + SoftReferenceNode.opSoftReferenceNode24.
 * 742 ServerProt.valueServerProt124 and 910 UPDATE_ZONE_PARTIAL_ENCLOSED use the same
 * remaining-length loop (g1 index into ZoneProt). Their header is level plus two
 * bytes the client shifts << 3. begin() has no level, so that byte is written as 0.
 * 317 has no such loop.
 */

struct rs_zonebuf {
    int revision;
    int begun;
    int base_x;
    int base_z;
    rs_buf nest;
};

rs_zonebuf *rs_zonebuf_create(int revision) {
    rs_zonebuf *z = rs_malloc(sizeof(*z));
    if (!z) {
        return NULL;
    }
    memset(z, 0, sizeof(*z));
    z->revision = revision;
    if (rs_buf_alloc(&z->nest, 64) != RS_OK) {
        free(z);
        return NULL;
    }
    return z;
}

void rs_zonebuf_destroy(rs_zonebuf *z) {
    if (!z) {
        return;
    }
    rs_buf_free(&z->nest);
    free(z);
}

rs_err rs_zonebuf_begin(rs_zonebuf *z, int base_x, int base_z) {
    if (!z) {
        return RS_ERR_ARG;
    }
    z->base_x = base_x;
    z->base_z = base_z;
    z->begun = 1;
    rs_buf_reset(&z->nest);
    return RS_OK;
}

rs_err rs_zonebuf_append(rs_zonebuf *z, int opcode, const uint8_t *body, size_t n) {
    rs_err e;
    if (!z || !z->begun || opcode < 0 || opcode > 255 || (n && !body)) {
        return RS_ERR_ARG;
    }
    e = rs_buf_ensure(&z->nest, 1 + n);
    if (e != RS_OK) {
        return e;
    }
    e = rs_p1(&z->nest, opcode);
    if (e != RS_OK) {
        return e;
    }
    if (!n) {
        return RS_OK;
    }
    return rs_pdata(&z->nest, body, n);
}

/* Scene tile -> the signed zone byte 742/910 shift << 3. */
static rs_err put_shifted(rs_buf *dst, int tile, int negated) {
    int zone;
    if (tile & 7) {
        return RS_ERR_RANGE;
    }
    zone = tile / 8;
    if (zone < -128 || zone > 127) {
        return RS_ERR_RANGE;
    }
    return negated ? rs_p1_neg(dst, zone) : rs_p1(dst, zone);
}

static rs_err put_nest(rs_buf *dst, const rs_zonebuf *z) {
    if (!z->nest.pos) {
        return RS_OK;
    }
    return rs_pdata(dst, z->nest.data, z->nest.pos);
}

rs_err rs_zonebuf_finish(rs_zonebuf *z, rs_buf *dst) {
    rs_err e;
    if (!z || !dst) {
        return RS_ERR_ARG;
    }
    /* 317 updates are top-level frames. */
    if (z->revision == 317) {
        return RS_ERR_UNSUPPORTED;
    }
    if (!z->begun) {
        return RS_ERR_ARG;
    }
    if (z->revision == 474) {
        e = rs_p1(dst, z->base_x);
        if (e != RS_OK) {
            return e;
        }
        e = rs_p1_sub128(dst, z->base_z);
        if (e != RS_OK) {
            return e;
        }
        return put_nest(dst, z);
    }
    if (z->revision == 530) {
        e = rs_p1_add128(dst, z->base_z);
        if (e != RS_OK) {
            return e;
        }
        e = rs_p1_sub128(dst, z->base_x);
        if (e != RS_OK) {
            return e;
        }
        return put_nest(dst, z);
    }
    if (z->revision == 742) {
        e = rs_p1_sub128(dst, 0);
        if (e != RS_OK) {
            return e;
        }
        e = put_shifted(dst, z->base_z, 1);
        if (e != RS_OK) {
            return e;
        }
        e = put_shifted(dst, z->base_x, 0);
        if (e != RS_OK) {
            return e;
        }
        return put_nest(dst, z);
    }
    if (z->revision == 910) {
        e = rs_p1_add128(dst, 0);
        if (e != RS_OK) {
            return e;
        }
        e = put_shifted(dst, z->base_z, 1);
        if (e != RS_OK) {
            return e;
        }
        e = put_shifted(dst, z->base_x, 1);
        if (e != RS_OK) {
            return e;
        }
        return put_nest(dst, z);
    }
    return RS_ERR_UNSUPPORTED;
}
