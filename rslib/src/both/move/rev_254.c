#include "rslib/both/move.h"

#include <string.h>

/* Client.tryMove writes opcode 6/220/127 as run, BE x, BE z, then dx/dz.
   The length byte is framing. 317 puts x, steps, z, then the run byte. */
static rs_err walk_encode(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    int i;
    rs_err e;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    if (w->nsteps < 0 || w->nsteps > RS_WALK_STEPS) {
        return RS_ERR_RANGE;
    }
    if (w->dest_x < 0 || w->dest_x > 65535 || w->dest_z < 0 || w->dest_z > 65535) {
        return RS_ERR_RANGE;
    }
    e = rs_p1(dst, w->running ? 1 : 0);
    if (e) {
        return e;
    }
    e = rs_p2(dst, w->dest_x);
    if (e) {
        return e;
    }
    e = rs_p2(dst, w->dest_z);
    if (e) {
        return e;
    }
    for (i = 0; i < w->nsteps; i++) {
        e = rs_p1(dst, w->dx[i]);
        if (e) {
            return e;
        }
        e = rs_p1(dst, w->dz[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static rs_err walk_decode(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 5) {
        return RS_ERR_NEED_MORE;
    }
    memset(w, 0, sizeof(*w));
    w->running = rs_g1(src) != 0;
    w->dest_x = rs_g2(src);
    w->dest_z = rs_g2(src);
    while (rs_buf_remaining(src) >= 2) {
        if (w->nsteps >= RS_WALK_STEPS) {
            return RS_ERR_RANGE;
        }
        w->dx[w->nsteps] = (int8_t)rs_g1(src);
        w->dz[w->nsteps] = (int8_t)rs_g1(src);
        w->nsteps++;
    }
    if (rs_buf_remaining(src) != 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

/* Client.read ptype 209: g2 zone x, g2 zone z. No local tile in the packet.
   sceneBaseTile = (zone - 6) * 8. ptype 73 is chat, not this rebuild. */
static rs_err region_encode(const rs_session *s, rs_buf *dst, const rs_region *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    if (v->zone_x < 0 || v->zone_x > 65535 || v->zone_z < 0 || v->zone_z > 65535) {
        return RS_ERR_RANGE;
    }
    e = rs_p2(dst, v->zone_x);
    if (e) {
        return e;
    }
    return rs_p2(dst, v->zone_z);
}

static rs_err region_decode(const rs_session *s, rs_buf *src, rs_region *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 4) {
        return RS_ERR_NEED_MORE;
    }
    memset(v, 0, sizeof(*v));
    v->zone_x = rs_g2(src);
    v->zone_z = rs_g2(src);
    return RS_OK;
}

static const rs_move_layout k_layout = {
    254,
    rs_sizes_c2s_254,
    rs_sizes_s2c_254,
    -1, /* RS_C2S_WALK already 6 */
    -1, /* RS_C2S_WALK_MINIMAP already 220 */
    209, /* Client.read ptype 209. The old bind 73 is chat in this client. */
    87, /* Client.read ptype 87 is readPlayerInfo. 81 is the 317 opcode. */
    -1,
    0,
    0,
    walk_encode,
    walk_decode,
    region_encode,
    region_decode,
    NULL,
    NULL};

void rs_move_254_init(void) {
    rs_move_register(&k_layout);
}

static void rs_move_254_ctor(void) __attribute__((constructor));
static void rs_move_254_ctor(void) {
    rs_move_254_init();
}

#ifdef RS_MOVE_TEST
#include "rslib/both/packets.h"

#include <stdio.h>

static int g_fail;

static void expect(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "fail: %s\n", msg);
        g_fail = 1;
    }
}

static void round_walk(void) {
    rs_buf dst, src;
    rs_walk in, out;
    static const uint8_t expect_bytes[] = {0x01, 0x0c, 0x97, 0x0c, 0x96, 0x01, 0xff};
    memset(&in, 0, sizeof in);
    in.running = 1;
    in.dest_x = 3223;
    in.dest_z = 3222;
    in.nsteps = 1;
    in.dx[0] = 1;
    in.dz[0] = -1;
    expect(rs_buf_alloc(&dst, 64) == RS_OK, "walk alloc");
    expect(walk_encode(NULL, &dst, &in) == RS_OK, "walk encode");
    expect(dst.pos == sizeof expect_bytes && memcmp(dst.data, expect_bytes, sizeof expect_bytes) == 0, "walk bytes");
    rs_buf_wrap(&src, dst.data, dst.pos);
    expect(walk_decode(NULL, &src, &out) == RS_OK, "walk decode");
    expect(out.running == 1 && out.dest_x == 3223 && out.dest_z == 3222, "walk dest");
    expect(out.nsteps == 1 && out.dx[0] == 1 && out.dz[0] == -1, "walk step");
    rs_buf_free(&dst);
}

static void round_region(void) {
    rs_buf dst, src;
    rs_region in, out;
    static const uint8_t expect_bytes[] = {0x01, 0x92, 0x01, 0x92};
    memset(&in, 0, sizeof in);
    in.zone_x = 402;
    in.zone_z = 402;
    in.local_x = 54;
    in.local_z = 54;
    expect(rs_buf_alloc(&dst, 16) == RS_OK, "region alloc");
    expect(region_encode(NULL, &dst, &in) == RS_OK, "region encode");
    expect(dst.pos == 4 && memcmp(dst.data, expect_bytes, 4) == 0, "region bytes");
    rs_buf_wrap(&src, dst.data, dst.pos);
    expect(region_decode(NULL, &src, &out) == RS_OK, "region decode");
    expect(out.zone_x == 402 && out.zone_z == 402, "region zones");
    expect(out.local_x == 0 && out.local_z == 0, "region has no local");
    rs_buf_free(&dst);
}

int main(void) {
    const rs_move_layout *m;
    const rs_build_info *info;
    rs_move_254_init();
    m = rs_move_for(254);
    expect(m != NULL, "registered");
    expect(m && m->walk_encode && m->walk_decode && m->region_encode && m->region_decode, "fns");
    expect(m && m->player_encode == NULL && m->player_decode == NULL, "player left null");
    expect(m && m->opcode_walk < 0 && m->opcode_region < 0 && m->opcode_player < 0, "opcodes already bound");
    info = rs_build_lookup(254);
    expect(rs_pkt_opcode(info, RS_C2S_WALK) == 6, "walk stays 6");
    expect(rs_pkt_opcode(info, RS_S2C_REGION) == 73, "region bind unchanged");
    round_walk();
    round_region();
    if (g_fail) {
        fprintf(stderr, "254 move test failed\n");
        return 1;
    }
    printf("254 move ok\n");
    return 0;
}
#endif
