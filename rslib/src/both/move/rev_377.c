#include "rslib/both/move.h"

#include <string.h>

/*
 * 377 Client.read opcode 222. Packet.g2(zone Z), Packet.g2_alt3(zone X).
 * Scene locals are not in the packet. Client base is (zone - 6) * 8.
 * Instanced rebuild is opcode 53 and is not this packet.
 * c2s and s2c size tables already live on the build. Walk opcode 28 is bound.
 *
 * Client.opClient135 is not the 317 walk order. Map 28, minimap 213,
 * interaction 247. Length byte is framing. The minimap's extra 14 bytes
 * are written by opClient125 after the walk body.
 * Dest X is p2_alt3, run is p1, dest Z is p2_alt3, steps are p1(dx) and
 * p1_alt3(dz).
 */

static rs_err region_encode_377(const rs_session *s, rs_buf *dst, const rs_region *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p2(dst, v->zone_z);
    if (e) {
        return e;
    }
    return rs_p2_le_add128(dst, v->zone_x);
}

static rs_err region_decode_377(const rs_session *s, rs_buf *src, rs_region *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 4) {
        return RS_ERR_FORMAT;
    }
    memset(v, 0, sizeof(*v));
    v->zone_z = rs_g2(src);
    v->zone_x = rs_g2_le_add128(src);
    v->local_x = 0;
    v->local_z = 0;
    return RS_OK;
}

static rs_err walk_encode_377(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    rs_err e;
    int i, n;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    n = w->nsteps;
    if (n > RS_WALK_STEPS) {
        n = RS_WALK_STEPS;
    }
    if (n < 0) {
        return RS_ERR_RANGE;
    }
    e = rs_p2_le_add128(dst, w->dest_x);
    if (e) {
        return e;
    }
    e = rs_p1(dst, w->running ? 1 : 0);
    if (e) {
        return e;
    }
    e = rs_p2_le_add128(dst, w->dest_z);
    if (e) {
        return e;
    }
    for (i = 0; i < n; i++) {
        e = rs_p1(dst, w->dx[i]);
        if (e) {
            return e;
        }
        e = rs_p1_sub128(dst, w->dz[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static rs_err walk_decode_377(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 5) {
        return RS_ERR_FORMAT;
    }
    memset(w, 0, sizeof(*w));
    w->dest_x = rs_g2_le_add128(src);
    w->running = rs_g1(src) != 0;
    w->dest_z = rs_g2_le_add128(src);
    while (rs_buf_remaining(src) >= 2 && w->nsteps < RS_WALK_STEPS) {
        w->dx[w->nsteps] = (int8_t)rs_g1(src);
        w->dz[w->nsteps] = (int8_t)rs_g1_sub128(src);
        w->nsteps++;
    }
    return RS_OK;
}

static const rs_move_layout k_377 = {
    377,
    NULL,
    NULL,
    28,
    213,
    222,
    -1,
    -1,
    0,
    0,
    walk_encode_377,
    walk_decode_377,
    region_encode_377,
    region_decode_377,
    NULL,
    NULL
};

__attribute__((constructor)) static void rs_move_377_init(void) {
    rs_move_register(&k_377);
}

#ifdef RS_MOVE_TEST
#include <stdio.h>

static int fail(const char *msg) {
    fprintf(stderr, "377 %s\n", msg);
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

int main(void) {
    rs_session session;
    rs_buf buf;
    rs_region in, out;
    rs_walk win, wout;
    const rs_move_layout *m;
    const rs_build_info *info;
    uint8_t raw[64];
    /* zone 402,402: g2(z) then g2_alt3(x). Pinned in test_milestone_wire. */
    static const uint8_t expect_lumbridge[] = {0x01, 0x92, 0x12, 0x01};
    static const uint8_t expect_region[] = {0x00, 0x14, 0x8a, 0x00};
    static const uint8_t expect_walk[] = {0x17, 0x0c, 0x01, 0x16, 0x0c, 0xff, 0x81, 0xfe, 0x7f};

    info = rs_build_lookup(377);
    if (!info) {
        return fail("lookup");
    }
    m = rs_move_for(377);
    if (!m || m->opcode_walk != 28 || m->region_encode == NULL || m->player_encode != NULL) {
        return fail("layout");
    }
    if (!rs_move_playable(info)) {
        return fail("playable");
    }
    memset(&session, 0, sizeof session);
    rs_session_init_role(&session, info, RS_ROLE_SERVER);
    memset(&in, 0, sizeof in);
    in.zone_x = 402;
    in.zone_z = 402;
    in.local_x = 54;
    in.local_z = 54;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (rs_codec_region_encode(&session, &buf, &in) != RS_OK || buf.pos != sizeof expect_lumbridge ||
        memcmp(raw, expect_lumbridge, sizeof expect_lumbridge) != 0) {
        return fail("lumbridge bytes");
    }
    rs_buf_wrap(&buf, raw, buf.pos);
    if (rs_codec_region_decode(&session, &buf, &out) != RS_OK || out.zone_x != 402 || out.zone_z != 402 ||
        out.local_x != 0 || out.local_z != 0) {
        return fail("lumbridge decode");
    }
    memset(&in, 0, sizeof in);
    in.zone_x = 10;
    in.zone_z = 20;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (rs_codec_region_encode(&session, &buf, &in) != RS_OK || buf.pos != sizeof expect_region ||
        memcmp(raw, expect_region, sizeof expect_region) != 0) {
        return fail("region bytes");
    }
    memset(&win, 0, sizeof win);
    win.dest_x = 3223;
    win.dest_z = 3222;
    win.running = 1;
    win.nsteps = 2;
    win.dx[0] = -1;
    win.dz[0] = -1;
    win.dx[1] = -2;
    win.dz[1] = 1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (rs_codec_walk_encode(&session, &buf, &win) != RS_OK || buf.pos != sizeof expect_walk ||
        memcmp(raw, expect_walk, sizeof expect_walk) != 0) {
        return fail("walk bytes");
    }
    rs_buf_wrap(&buf, raw, buf.pos);
    if (rs_codec_walk_decode(&session, &buf, &wout) != RS_OK || !same_walk(&win, &wout)) {
        return fail("walk decode");
    }
    return 0;
}
#endif
