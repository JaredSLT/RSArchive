#include "rslib/both/move.h"

#include <string.h>

/*
 * 350 Client.read opcode 152. C9cd2e2e1.opkind8743 is g2_alt3 while
 * valueClient615 is negative (-151 / -303). Zone Z, then zone X.
 * Scene locals are not in the packet. Client base is (zone - 6) * 8.
 * Instanced rebuild is opcode 20 and is not this packet.
 * Player info is Client.read opcode 37 (readPlayerInfo). Size is -2.
 *
 * No c2s int[256] in the client. Incoming sizes are C994ec50d.valuekind1192
 * only, already stored as s2c. sizes_c2s is the write-derived table below.
 *
 * Client.opClient92 walk body is not the 317 order. Map opcode 246,
 * minimap 147, interaction 216. Length byte is framing, not the body.
 * Run is p1_alt1, dest Z is p2_alt1, steps are p1_alt1(dx) and p1_alt2(dz),
 * dest X is p2_alt1.
 */

static rs_err region_encode_350(const rs_session *s, rs_buf *dst, const rs_region *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p2_le_add128(dst, v->zone_z);
    if (e) {
        return e;
    }
    return rs_p2_le_add128(dst, v->zone_x);
}

static rs_err region_decode_350(const rs_session *s, rs_buf *src, rs_region *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 4) {
        return RS_ERR_FORMAT;
    }
    memset(v, 0, sizeof(*v));
    v->zone_z = rs_g2_le_add128(src);
    v->zone_x = rs_g2_le_add128(src);
    v->local_x = 0;
    v->local_z = 0;
    return RS_OK;
}

static rs_err walk_encode_350(const rs_session *s, rs_buf *dst, const rs_walk *w) {
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
    e = rs_p1_add128(dst, w->running ? 1 : 0);
    if (e) {
        return e;
    }
    e = rs_p2_le(dst, w->dest_z);
    if (e) {
        return e;
    }
    for (i = 0; i < n; i++) {
        e = rs_p1_add128(dst, w->dx[i]);
        if (e) {
            return e;
        }
        e = rs_p1_neg(dst, w->dz[i]);
        if (e) {
            return e;
        }
    }
    return rs_p2_le(dst, w->dest_x);
}

static rs_err walk_decode_350(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 5) {
        return RS_ERR_FORMAT;
    }
    memset(w, 0, sizeof(*w));
    w->running = rs_g1_add128(src) != 0;
    w->dest_z = rs_g2_le(src);
    while ((int)rs_buf_remaining(src) >= 4 && w->nsteps < RS_WALK_STEPS) {
        w->dx[w->nsteps] = (int8_t)rs_g1_add128(src);
        w->dz[w->nsteps] = (int8_t)rs_g1_neg(src);
        w->nsteps++;
    }
    w->dest_x = rs_g2_le(src);
    return RS_OK;
}

/* Payload after the opcode. -1 is one length byte, -3 was not written.
 * 246 and 147 are walk_encode_350 with nsteps == 0. The client also writes a
 * length byte, and minimap callers add 14 bytes this encoder does not write.
 */
static const int8_t k_c2s_350[256] = {
     -3,   2,  -3,  10,  -3,  -3,  -3,  -3,  -3,   6,  -3,  -3,   6,  -3,   8,  -3,
      2,   6,  12,   6,  -3,   4,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,   4,  -3,
     -3,  -3,  -3,   0,  -3,   2,  -3,  -3,  -3,   6,  -3,  -3,  -3,  -3,  -3,  -3,
      8,  -3,  -3,  -3,  -3,  -3,   2,  -3,  -3,   6,   0,   6,  -3,  -3,  -3,   0,
     -3,  -3,  -3,  -3,  -3,  -3,  -3,   8,  -3,   6,  -3,  -3,   2,  -3,  -3,  -3,
     -3,  -3,  -3,  -3,   4,  -3,   4,   2,  -3,  -3,   8,  -3,  -3,   6,  -3,   4,
     -3,  -3,  -3,   2,   0,  -3,   6,  -3,   8,   6,  -3,  -3,  -3,   7,  -3,  -3,
     -3,  12,  -3,  -3,  -3,   6,  -3,  -1,  -3,  -3,  -3,  -3,   3,  -3,  -3,  -1,
     -3,  -3,   4,  -3,  -3,  -3,   2,  -3,  -3,  12,  -3,   1,   6,  -3,  -3,   6,
     -3,  -3,   8,   5,  -3,  -3,   4,  -3,  -3,  -3,   2,  -3,  -3,  -3,   2,  -3,
     -3,  -3,   2,   2,   3,  -3,  -3,   8,  -1,  -1,   4,  -3,  -3,   4,   6,  -3,
      6,  -3,  -3,  -3,  -3,  -3,   0,  -3,  -3,  -3,  -3,  -3,  -3,   8,   2,  -3,
     -3,   4,  -3,  -3,  -3,  -3,  -3,   4,   8,  -3,  -3,  -3,  -3,  13,   6,  -3,
      2,  -3,   0,   0,  -3,  -3,  -3,  -3,  -1,   6,  -3,  -3,  -3,  -3,  -3,   8,
     -1,   1,   6,  -3,  -3,  -3,  -1,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,
     -3,  -3,  -3,  -3,  -3,  -3,   5,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,   6,
};

static const rs_move_layout k_350 = {
    350,
    k_c2s_350,
    NULL,
    246,
    147,
    152,
    37, /* Client.read -> readPlayerInfo */
    -1,
    0,
    0,
    walk_encode_350,
    walk_decode_350,
    region_encode_350,
    region_decode_350,
    NULL,
    NULL
};

__attribute__((constructor)) static void rs_move_350_init(void) {
    rs_move_register(&k_350);
}

#ifdef RS_MOVE_TEST
#include <stdio.h>

static int fail(const char *msg) {
    fprintf(stderr, "350 %s\n", msg);
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
    static const uint8_t expect_region[] = {0x94, 0x00, 0x8a, 0x00};
    static const uint8_t expect_walk[] = {0x81, 0x96, 0x0c, 0x7f, 0x01, 0x7e, 0xff, 0x97, 0x0c};

    info = rs_build_lookup(350);
    if (!info) {
        return fail("lookup");
    }
    m = rs_move_for(350);
    if (!m || m->sizes_c2s != k_c2s_350 || m->opcode_walk != 246 || m->region_encode == NULL || m->player_encode != NULL) {
        return fail("layout");
    }
    {
        rs_walk z;
        memset(&z, 0, sizeof z);
        z.dest_x = 3222;
        z.dest_z = 3222;
        rs_buf_wrap(&buf, raw, sizeof raw);
        if (walk_encode_350(NULL, &buf, &z) != RS_OK || m->sizes_c2s[m->opcode_walk] != (int)buf.pos) {
            return fail("c2s walk size");
        }
        if (m->sizes_c2s[0] != -3) {
            return fail("c2s unknown");
        }
    }
    if (rs_move_playable(info) != 1) {
        return fail("playable");
    }
    memset(&session, 0, sizeof session);
    rs_session_init_role(&session, info, RS_ROLE_SERVER);
    memset(&in, 0, sizeof in);
    in.zone_x = 10;
    in.zone_z = 20;
    in.local_x = 54;
    in.local_z = 54;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (rs_codec_region_encode(&session, &buf, &in) != RS_OK || buf.pos != sizeof expect_region ||
        memcmp(raw, expect_region, sizeof expect_region) != 0) {
        return fail("region bytes");
    }
    rs_buf_wrap(&buf, raw, buf.pos);
    if (rs_codec_region_decode(&session, &buf, &out) != RS_OK || out.zone_x != 10 || out.zone_z != 20 ||
        out.local_x != 0 || out.local_z != 0) {
        return fail("region decode");
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
