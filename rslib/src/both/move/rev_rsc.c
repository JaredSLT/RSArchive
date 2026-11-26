#include "rslib/both/move.h"

#include <string.h>

/*
 * RSC has no RS2 zone rebuild and no dir1/dir2 scene step for the local player.
 *
 * Walk body is MudClient.call4534748f (115 mudclient.zl, 159 mudclient.xl,
 * 177 mudclient.a). Walk-here is opcode 255 on 115 and 194 on 159 and 177.
 * The minimap click (115 call0311411f at the compass, var5 false) uses that
 * same opcode and body, so opcode_walk_minimap stays unset. Approach is
 * opcode 215 on all three and is not this packet. RS_C2S_WALK is already
 * bound to 221 (cast-on-ground), so opcode_walk is -1.
 * Two big-endian shorts are the destination in the same coordinate space as
 * opcode 255, then up to 25 pairs (pathX - destX, pathY - destY) written
 * from the point before the destination backwards. No run byte.
 *
 * Local player is MudClient.call5fac7c41 opcode 255 (115 starts at bit 8 of
 * the framed buffer, which is bit 0 of the payload). 115 is 10-bit x and
 * 12-bit y; 159 and 177 are 11 and 13. Then a 4-bit number18 and an 8-bit
 * count of other players. This codec writes the count as 0 and does not
 * encode other players. The 4-bit is the client's facing, not the NW..SE
 * step index, and is stored in dir1. dir2 is unused. calla598fe3b always
 * takes the absolute tile, so teleport is 1 and teleport 0 is unsupported.
 * Appearance is opcode 250 type 5, not this packet. RS_S2C_PLAYER_UPDATE is
 * already 191, so opcode_player is -1.
 *
 * There is no region opcode. call8390e4c9 (115 mudclient.tm, 159 mudclient.um)
 * loads the map from those same tiles. Opcode 244 offsets start at 0 and are
 * not in this prefix, so the origin is ((tile + 24) / 48) * 48 - 48.
 * zone = origin / 8 + 6 and local = tile - origin, which makes
 * (zone - 6) * 8 + local equal the tile. 177 call8390e4c9 did not decompile
 * (the method returns false). The position handler still calls it with the
 * 159-width coordinates, and the sector expression is the 115/159 one.
 * RS_S2C_REGION is already 25, which is not this prefix, so opcode_region
 * is -1. The client assigns no start tile (115 call5fac7c41, 159 the
 * 11-bit position read, 177 call8390e4c9's caller). Spawn is 100,200,
 * the pair RS_MOVE_TEST already round-trips inside 10/12 and 11/13 bits.
 * Framing is Stream.readPacket, not an int[256] size table.
 */

static int coord_bits(int revision, int *xbits, int *ybits) {
    if (revision == 115) {
        *xbits = 10;
        *ybits = 12;
        return 0;
    }
    if (revision == 159 || revision == 177) {
        *xbits = 11;
        *ybits = 13;
        return 0;
    }
    return -1;
}

static int session_rev(const rs_session *s) {
    if (!s || !s->build) {
        return 0;
    }
    return s->build->revision;
}

static rs_err put_bits(rs_buf *b, int n, int v) {
    size_t off;
    size_t end;
    rs_err e;
    if (!b || n < 0) {
        return RS_ERR_ARG;
    }
    e = rs_buf_ensure(b, (size_t)((n + 7) / 8) + 2);
    if (e) {
        return e;
    }
    off = b->bit_pos >> 3;
    end = (b->bit_pos + (size_t)n + 7) >> 3;
    if ((b->bit_pos & 7) == 0) {
        if (end > off && end <= b->cap) {
            memset(b->data + off, 0, end - off);
        }
    } else if (end > off + 1 && end <= b->cap) {
        memset(b->data + off + 1, 0, end - (off + 1));
    }
    return rs_pbit(b, n, v);
}

static rs_err get_bits(rs_buf *b, int n, int *out) {
    size_t end;
    if (!b || !b->data || n < 0) {
        return RS_ERR_ARG;
    }
    end = b->cap * 8;
    if (b->bit_pos > end || (size_t)n > end - b->bit_pos) {
        return RS_ERR_RANGE;
    }
    *out = rs_gbit(b, n);
    return RS_OK;
}

static rs_err read_tile(rs_buf *src, int revision, int *x, int *z) {
    int xbits;
    int ybits;
    rs_err e;
    if (coord_bits(revision, &xbits, &ybits)) {
        return RS_ERR_UNSUPPORTED;
    }
    rs_access_bits(src);
    e = get_bits(src, xbits, x);
    if (e) {
        return e;
    }
    return get_bits(src, ybits, z);
}

static rs_err write_tile(rs_buf *dst, int revision, int x, int z) {
    int xbits;
    int ybits;
    rs_err e;
    if (coord_bits(revision, &xbits, &ybits)) {
        return RS_ERR_UNSUPPORTED;
    }
    if (x < 0 || z < 0 || x >= (1 << xbits) || z >= (1 << ybits)) {
        return RS_ERR_RANGE;
    }
    rs_access_bits(dst);
    e = put_bits(dst, xbits, x);
    if (e) {
        return e;
    }
    return put_bits(dst, ybits, z);
}

/* call8390e4c9 with opcode 244 offsets still 0. */
static void sector_split(int tile, int *zone, int *local) {
    int origin = ((tile + 24) / 48) * 48 - 48;
    *local = tile - origin;
    *zone = origin / 8 + 6;
}

static int tile_of(int zone, int local, int *tile) {
    int w = (zone - 6) * 8 + local;
    if (w < 0) {
        return -1;
    }
    *tile = w;
    return 0;
}

static rs_err walk_encode(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    int i;
    int n;
    rs_err e;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    n = w->nsteps;
    if (n < 0 || n > RS_WALK_STEPS) {
        return RS_ERR_RANGE;
    }
    if (w->dest_x < 0 || w->dest_x > 65535 || w->dest_z < 0 || w->dest_z > 65535) {
        return RS_ERR_RANGE;
    }
    e = rs_p2(dst, w->dest_x);
    if (e) {
        return e;
    }
    e = rs_p2(dst, w->dest_z);
    if (e) {
        return e;
    }
    for (i = 0; i < n; i++) {
        e = rs_p1(dst, w->dx[i] & 0xff);
        if (e) {
            return e;
        }
        e = rs_p1(dst, w->dz[i] & 0xff);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static int sign8(int v) {
    v &= 0xff;
    return v >= 128 ? v - 256 : v;
}

static rs_err walk_decode(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 4) {
        return RS_ERR_FORMAT;
    }
    memset(w, 0, sizeof(*w));
    w->dest_x = rs_g2(src);
    w->dest_z = rs_g2(src);
    while (rs_buf_remaining(src) >= 2 && w->nsteps < RS_WALK_STEPS) {
        w->dx[w->nsteps] = (int8_t)sign8(rs_g1(src));
        w->dz[w->nsteps] = (int8_t)sign8(rs_g1(src));
        w->nsteps++;
    }
    return RS_OK;
}

static rs_err region_encode(const rs_session *s, rs_buf *dst, const rs_region *v) {
    int x;
    int z;
    rs_err e;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    if (tile_of(v->zone_x, v->local_x, &x) || tile_of(v->zone_z, v->local_z, &z)) {
        return RS_ERR_RANGE;
    }
    e = write_tile(dst, session_rev(s), x, z);
    if (e) {
        return e;
    }
    rs_access_bytes(dst);
    return RS_OK;
}

static rs_err region_decode(const rs_session *s, rs_buf *src, rs_region *v) {
    int x;
    int z;
    rs_err e;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    e = read_tile(src, session_rev(s), &x, &z);
    if (e) {
        return e;
    }
    rs_access_bytes(src);
    sector_split(x, &v->zone_x, &v->local_x);
    sector_split(z, &v->zone_z, &v->local_z);
    return RS_OK;
}

static rs_err player_encode(const rs_session *s, rs_buf *dst, const rs_move_step *step) {
    int rev;
    int face;
    rs_err e;
    /* Absolute tile. The play host passes that in local_x/local_z for RSC
     * (see play.c). Scene local alone cannot recover it; do not invent a base. */
    if (!dst || !step) {
        return RS_ERR_ARG;
    }
    if (step->teleport != 1) {
        return RS_ERR_UNSUPPORTED;
    }
    face = step->dir1 < 0 ? 0 : step->dir1;
    if (face > 15) {
        return RS_ERR_RANGE;
    }
    rev = session_rev(s);
    e = write_tile(dst, rev, step->local_x, step->local_z);
    if (e) {
        return e;
    }
    e = put_bits(dst, 4, face);
    if (e) {
        return e;
    }
    e = put_bits(dst, 8, 0);
    if (e) {
        return e;
    }
    rs_access_bytes(dst);
    return RS_OK;
}

static rs_err player_decode(const rs_session *s, rs_buf *src, rs_move_step *step) {
    int x;
    int z;
    int face;
    int known;
    int zone_x;
    int zone_z;
    rs_err e;
    if (!src || !step) {
        return RS_ERR_ARG;
    }
    memset(step, 0, sizeof(*step));
    e = read_tile(src, session_rev(s), &x, &z);
    if (e) {
        return e;
    }
    e = get_bits(src, 4, &face);
    if (e) {
        return e;
    }
    e = get_bits(src, 8, &known);
    if (e) {
        return e;
    }
    (void)known;
    rs_access_bytes(src);
    sector_split(x, &zone_x, &step->local_x);
    sector_split(z, &zone_z, &step->local_z);
    (void)zone_x;
    (void)zone_z;
    step->teleport = 1;
    step->dir1 = face;
    step->dir2 = -1;
    return RS_OK;
}

static const rs_move_layout k_115 = {
    115, NULL, NULL, -1, -1, -1, -1, -1, 100, 200, walk_encode, walk_decode, region_encode, region_decode,
    player_encode, player_decode};

static const rs_move_layout k_159 = {
    159, NULL, NULL, -1, -1, -1, -1, -1, 100, 200, walk_encode, walk_decode, region_encode, region_decode,
    player_encode, player_decode};

static const rs_move_layout k_177 = {
    177, NULL, NULL, -1, -1, -1, -1, -1, 100, 200, walk_encode, walk_decode, region_encode, region_decode,
    player_encode, player_decode};

__attribute__((constructor)) static void rs_move_rsc_init(void) {
    rs_move_register(&k_115);
    rs_move_register(&k_159);
    rs_move_register(&k_177);
}

#ifdef RS_MOVE_TEST
#include <stdio.h>
#include <stdlib.h>

static int fail(const char *msg) {
    fprintf(stderr, "rsc %s\n", msg);
    return 1;
}

static int same_walk(const rs_walk *a, const rs_walk *b) {
    int i;
    if (a->dest_x != b->dest_x || a->dest_z != b->dest_z || a->running != b->running || a->nsteps != b->nsteps) {
        return 0;
    }
    for (i = 0; i < a->nsteps; i++) {
        if (a->dx[i] != b->dx[i] || a->dz[i] != b->dz[i]) {
            return 0;
        }
    }
    return 1;
}

static int scene_is(int abs_x, int abs_z, const rs_move_step *step) {
    int zone_x;
    int local_x;
    int zone_z;
    int local_z;
    sector_split(abs_x, &zone_x, &local_x);
    sector_split(abs_z, &zone_z, &local_z);
    return step->local_x == local_x && step->local_z == local_z &&
           (zone_x - 6) * 8 + step->local_x == abs_x && (zone_z - 6) * 8 + step->local_z == abs_z;
}

static int roundtrip(int revision) {
    rs_session session;
    rs_buf buf;
    rs_region region;
    rs_region region_out;
    rs_walk walk;
    rs_walk walk_out;
    rs_move_step step;
    rs_move_step step_out;
    const rs_move_layout *m;
    const rs_build_info *info;
    uint8_t raw[64];
    int xbits;
    int ybits;
    static const uint8_t walk_bytes[] = {0x0c, 0x96, 0x0c, 0x97, 0xff, 0x01};
    static const uint8_t region_115[] = {0x19, 0x03, 0x20};
    static const uint8_t region_159[] = {0x0c, 0x80, 0xc8};
    static const uint8_t place_115[] = {0x19, 0x03, 0x20, 0x00, 0x00};
    static const uint8_t place_159[] = {0x0c, 0x80, 0xc8, 0x00, 0x00};
    static const uint8_t step_115[] = {0x19, 0x43, 0x20, 0x80, 0x00};
    static const uint8_t step_159[] = {0x0c, 0xa0, 0xc8, 0x20, 0x00};
    const uint8_t *region_expect;
    const uint8_t *place_expect;
    const uint8_t *step_expect;
    size_t region_n;
    size_t place_n;
    size_t step_n;

    info = rs_build_lookup(revision);
    if (!info) {
        return fail("lookup");
    }
    m = rs_move_for(revision);
    if (!m || m->sizes_c2s || m->sizes_s2c || m->opcode_walk != -1 || m->opcode_region != -1 ||
        m->opcode_player != -1 || m->spawn_x != 100 || m->spawn_z != 200 || !m->walk_encode ||
        !m->region_encode || !m->player_encode) {
        return fail("layout");
    }
    if (coord_bits(revision, &xbits, &ybits)) {
        return fail("bits");
    }
    if (revision == 115) {
        region_expect = region_115;
        region_n = sizeof region_115;
        place_expect = place_115;
        place_n = sizeof place_115;
        step_expect = step_115;
        step_n = sizeof step_115;
    } else {
        region_expect = region_159;
        region_n = sizeof region_159;
        place_expect = place_159;
        place_n = sizeof place_159;
        step_expect = step_159;
        step_n = sizeof step_159;
    }
    memset(&session, 0, sizeof session);
    rs_session_init_role(&session, info, RS_ROLE_SERVER);

    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.local_x = 100;
    step.local_z = 200;
    step.dir1 = 0;
    step.dir2 = -1;
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (m->player_encode(&session, &buf, &step) != RS_OK || buf.pos != place_n ||
        memcmp(raw, place_expect, place_n) != 0) {
        return fail("teleport bytes");
    }
    rs_buf_wrap(&buf, raw, buf.pos);
    if (m->player_decode(&session, &buf, &step_out) != RS_OK || step_out.teleport != 1 ||
        !scene_is(100, 200, &step_out) || step_out.dir1 != 0 || step_out.dir2 != -1 ||
        step_out.appearance != 0) {
        return fail("teleport decode");
    }

    /* One tile east of the placement. The client stores the new absolute tile. */
    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.local_x = 101;
    step.local_z = 200;
    step.dir1 = 2;
    step.dir2 = -1;
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (m->player_encode(&session, &buf, &step) != RS_OK || buf.pos != step_n ||
        memcmp(raw, step_expect, step_n) != 0) {
        return fail("step bytes");
    }
    rs_buf_wrap(&buf, raw, buf.pos);
    if (m->player_decode(&session, &buf, &step_out) != RS_OK || step_out.teleport != 1 ||
        !scene_is(101, 200, &step_out) || step_out.dir1 != 2) {
        return fail("step decode");
    }
    step.teleport = 0;
    step.dir1 = 4;
    step.dir2 = -1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (m->player_encode(&session, &buf, &step) != RS_ERR_UNSUPPORTED) {
        return fail("dir step");
    }

    memset(&region, 0, sizeof region);
    sector_split(100, &region.zone_x, &region.local_x);
    sector_split(200, &region.zone_z, &region.local_z);
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (rs_codec_region_encode(&session, &buf, &region) != RS_OK || buf.pos != region_n ||
        memcmp(raw, region_expect, region_n) != 0) {
        return fail("region bytes");
    }
    rs_buf_wrap(&buf, raw, buf.pos);
    if (rs_codec_region_decode(&session, &buf, &region_out) != RS_OK || region_out.zone_x != region.zone_x ||
        region_out.zone_z != region.zone_z || region_out.local_x != region.local_x ||
        region_out.local_z != region.local_z) {
        return fail("region decode");
    }

    memset(&walk, 0, sizeof walk);
    walk.dest_x = 3222;
    walk.dest_z = 3223;
    walk.nsteps = 1;
    walk.dx[0] = -1;
    walk.dz[0] = 1;
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (rs_codec_walk_encode(&session, &buf, &walk) != RS_OK || buf.pos != sizeof walk_bytes ||
        memcmp(raw, walk_bytes, sizeof walk_bytes) != 0) {
        return fail("walk bytes");
    }
    rs_buf_wrap(&buf, raw, buf.pos);
    if (rs_codec_walk_decode(&session, &buf, &walk_out) != RS_OK || !same_walk(&walk, &walk_out)) {
        return fail("walk decode");
    }
    return 0;
}

int main(void) {
    if (roundtrip(115) || roundtrip(159) || roundtrip(177)) {
        return 1;
    }
    return 0;
}
#endif
