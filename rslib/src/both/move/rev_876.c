#include "rslib/both/move.h"

#include <string.h>

/* Friend.doFriend: p2(x), p2_alt1(z), p1_alt3(ctrl). Size 5, no steps. */
static rs_err walk_encode(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    if (w->nsteps != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if (rs_p2(dst, w->dest_x) != RS_OK) {
        return RS_ERR_RANGE;
    }
    if (rs_p2_le(dst, w->dest_z) != RS_OK) {
        return RS_ERR_RANGE;
    }
    return rs_p1_sub128(dst, w->running ? 1 : 0);
}

static rs_err walk_decode(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 5) {
        return RS_ERR_RANGE;
    }
    memset(w, 0, sizeof(*w));
    w->dest_x = rs_g2(src);
    w->dest_z = rs_g2_le(src);
    w->running = rs_g1_sub128(src) != 0;
    return RS_OK;
}

/* MapAlignmentY: 0=104, 1=120, 2=136, 3=168, 4=72, 5=256. */
static int area_tiles(int id) {
    switch (id) {
    case 0:
        return 104;
    case 1:
        return 120;
    case 2:
        return 136;
    case 3:
        return 168;
    case 4:
        return 72;
    case 5:
        return 256;
    default:
        return -1;
    }
}

static int square_count(int zone_x, int zone_z, int tiles) {
    int rad = tiles >> 4;
    int mx0 = (zone_x - rad) / 8;
    int mx1 = (zone_x + rad) / 8;
    int mz0 = (zone_z - rad) / 8;
    int mz1 = (zone_z + rad) / 8;
    return (mx1 - mx0 + 1) * (mz1 - mz0 + 1);
}

/*
 * World.doWorld(PositionedSoundPart4): g2 z, g1 build, g2_alt1 x, g1_alt1 force,
 * g1_alt1 count, g1_alt1 npc view. No XTEA.
 */
static rs_err region_encode(const rs_session *s, rs_buf *dst, const rs_region *v) {
    int tiles, n;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    tiles = area_tiles(v->build_area);
    if (tiles < 0) {
        return RS_ERR_RANGE;
    }
    n = square_count(v->zone_x, v->zone_z, tiles);
    if (n < 0 || n > 255) {
        return RS_ERR_RANGE;
    }
    if (rs_p2(dst, v->zone_z) != RS_OK) {
        return RS_ERR_RANGE;
    }
    if (rs_p1(dst, v->build_area) != RS_OK) {
        return RS_ERR_RANGE;
    }
    if (rs_p2_le(dst, v->zone_x) != RS_OK) {
        return RS_ERR_RANGE;
    }
    if (rs_p1_add128(dst, v->force_reload ? 1 : 0) != RS_OK) {
        return RS_ERR_RANGE;
    }
    if (rs_p1_add128(dst, n) != RS_OK) {
        return RS_ERR_RANGE;
    }
    return rs_p1_add128(dst, v->npc_view);
}

static rs_err region_decode(const rs_session *s, rs_buf *src, rs_region *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 8) {
        return RS_ERR_RANGE;
    }
    memset(v, 0, sizeof(*v));
    v->zone_z = rs_g2(src);
    v->build_area = rs_g1(src);
    v->zone_x = rs_g2_le(src);
    v->force_reload = rs_g1_add128(src) == 1;
    (void)rs_g1_add128(src);
    v->npc_view = rs_g1_add128(src);
    if (area_tiles(v->build_area) < 0) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

static const int k_rs3 = 1;

/* Direction wire is SW S SE W E NW N NE. Contract order is NW N NE W E SW S SE. */
static const int k_wire[8] = {5, 6, 7, 3, 4, 0, 1, 2};
static const int k_dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
static const int k_dz[8] = {1, 1, 1, 0, 0, -1, -1, -1};
static const int k_run_dx[16] = {-2, -1, 0, 1, 2, -2, 2, -2, 2, -2, 2, -2, -1, 0, 1, 2};
static const int k_run_dz[16] = {-2, -2, -2, -2, -2, -1, -1, 0, 0, 1, 1, 2, 2, 2, 2, 2};
/* Post-snapshot nsn 0: one high-res local player, 2046 low-res indices. */
static const int k_low_skip = 2045;

static int run_code(int d1, int d2) {
    int x = k_dx[d1] + k_dx[d2];
    int z = k_dz[d1] + k_dz[d2];
    int i;
    for (i = 0; i < 16; i++) {
        if (k_run_dx[i] == x && k_run_dz[i] == z) {
            return i;
        }
    }
    return -1;
}

static int split_run(int code, int *d1, int *d2) {
    int a, b;
    if (code < 0 || code > 15) {
        return 0;
    }
    for (a = 0; a < 8; a++) {
        for (b = 0; b < 8; b++) {
            if (k_dx[a] + k_dx[b] == k_run_dx[code] && k_dz[a] + k_dz[b] == k_run_dz[code]) {
                *d1 = a;
                *d2 = b;
                return 1;
            }
        }
    }
    return 0;
}

static rs_err put_skip(rs_buf *b, int n) {
    rs_err e;
    if (n == 0) {
        return rs_pbit(b, 2, 0);
    }
    if (n < 32) {
        e = rs_pbit(b, 2, 1);
        if (e) {
            return e;
        }
        return rs_pbit(b, 5, n);
    }
    if (n < 256) {
        e = rs_pbit(b, 2, 2);
        if (e) {
            return e;
        }
        return rs_pbit(b, 8, n);
    }
    if (n < 2048) {
        e = rs_pbit(b, 2, 3);
        if (e) {
            return e;
        }
        return rs_pbit(b, 11, n);
    }
    return RS_ERR_RANGE;
}

static int get_skip(rs_buf *b) {
    int kind = rs_gbit(b, 2);
    if (kind == 0) {
        return 0;
    }
    if (kind == 1) {
        return rs_gbit(b, 5);
    }
    if (kind == 2) {
        return rs_gbit(b, 8);
    }
    return rs_gbit(b, 11);
}

/* kind: 0 idle, 1 one step, 2 two-tile, 3 teleport. */
static rs_err classify(const rs_move_step *step, int *kind, int *code) {
    if (step->teleport) {
        if (step->dir1 != -1 || step->dir2 != -1) {
            return RS_ERR_UNSUPPORTED;
        }
        if (step->plane < 0 || step->plane > 3) {
            return RS_ERR_RANGE;
        }
        if (step->local_x < 0 || step->local_x > 16383 || step->local_z < 0 || step->local_z > 16383) {
            return RS_ERR_RANGE;
        }
        *kind = 3;
        return RS_OK;
    }
    if (step->dir1 == -1 && step->dir2 == -1) {
        *kind = 0;
        return RS_OK;
    }
    if (step->dir1 < 0 || step->dir1 > 7) {
        return RS_ERR_UNSUPPORTED;
    }
    if (step->dir2 == -1) {
        *kind = 1;
        return RS_OK;
    }
    if (step->dir2 < 0 || step->dir2 > 7) {
        return RS_ERR_UNSUPPORTED;
    }
    *code = run_code(step->dir1, step->dir2);
    if (*code < 0) {
        return RS_ERR_UNSUPPORTED;
    }
    *kind = 2;
    return RS_OK;
}

static rs_err put_move(rs_buf *dst, const rs_move_step *step, int kind, int code) {
    rs_err e;
    /* Appearance mask bytes were not fully read, so the extend bit stays clear. */
    e = rs_pbit(dst, 1, 0);
    if (e) {
        return e;
    }
    if (kind == 1) {
        e = rs_pbit(dst, 2, 1);
        if (e) {
            return e;
        }
        e = rs_pbit(dst, 3, k_wire[step->dir1]);
        if (e) {
            return e;
        }
        if (k_rs3) {
            return rs_pbit(dst, 1, 0);
        }
        return RS_OK;
    }
    if (kind == 2) {
        e = rs_pbit(dst, 2, 2);
        if (e) {
            return e;
        }
        return rs_pbit(dst, 4, code);
    }
    e = rs_pbit(dst, 2, 3);
    if (e) {
        return e;
    }
    e = rs_pbit(dst, 1, 1);
    if (e) {
        return e;
    }
    if (k_rs3) {
        e = rs_pbit(dst, 3, 4);
        if (e) {
            return e;
        }
    }
    /* Java adds the 14-bit fields to the current waypoint, then subtracts the region base. */
    return rs_pbit(dst, 30, (step->plane << 28) | (step->local_x << 14) | step->local_z);
}

static rs_err player_encode(const rs_session *s, rs_buf *dst, const rs_move_step *step) {
    int kind = 0;
    int code = 0;
    rs_err e;
    (void)s;
    if (!dst || !step) {
        return RS_ERR_ARG;
    }
    e = classify(step, &kind, &code);
    if (e) {
        return e;
    }
    rs_access_bits(dst);
    if (kind == 0) {
        e = rs_pbit(dst, 1, 0);
        if (e) {
            return e;
        }
        e = put_skip(dst, 0);
        if (e) {
            return e;
        }
    } else {
        e = rs_pbit(dst, 1, 1);
        if (e) {
            return e;
        }
        e = put_move(dst, step, kind, code);
        if (e) {
            return e;
        }
    }
    rs_access_bytes(dst);
    rs_access_bits(dst);
    rs_access_bytes(dst);
    rs_access_bits(dst);
    rs_access_bytes(dst);
    rs_access_bits(dst);
    e = rs_pbit(dst, 1, 0);
    if (e) {
        return e;
    }
    e = put_skip(dst, k_low_skip);
    if (e) {
        return e;
    }
    rs_access_bytes(dst);
    return RS_OK;
}

static rs_err take_move(rs_buf *src, rs_move_step *step) {
    int type;
    int code;
    int word;
    if (rs_gbit(src, 1) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    type = rs_gbit(src, 2);
    step->dir1 = -1;
    step->dir2 = -1;
    step->teleport = 0;
    if (type == 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if (type == 1) {
        step->dir1 = k_wire[rs_gbit(src, 3)];
        if (k_rs3 && rs_gbit(src, 1) != 0) {
            (void)rs_gbit(src, 2);
            return RS_ERR_UNSUPPORTED;
        }
        return RS_OK;
    }
    if (type == 2) {
        code = rs_gbit(src, 4);
        if (!split_run(code, &step->dir1, &step->dir2)) {
            return RS_ERR_FORMAT;
        }
        return RS_OK;
    }
    if (rs_gbit(src, 1) == 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if (k_rs3) {
        (void)rs_gbit(src, 3);
    }
    word = rs_gbit(src, 30);
    step->teleport = 1;
    step->plane = (word >> 28) & 3;
    step->local_x = (word >> 14) & 16383;
    step->local_z = word & 16383;
    return RS_OK;
}

static rs_err player_decode(const rs_session *s, rs_buf *src, rs_move_step *step) {
    int bit;
    rs_err e;
    (void)s;
    if (!src || !step) {
        return RS_ERR_ARG;
    }
    memset(step, 0, sizeof(*step));
    step->dir1 = -1;
    step->dir2 = -1;
    rs_access_bits(src);
    bit = rs_gbit(src, 1);
    if (bit == 0) {
        if (get_skip(src) != 0) {
            return RS_ERR_UNSUPPORTED;
        }
    } else {
        e = take_move(src, step);
        if (e) {
            return e;
        }
    }
    rs_access_bytes(src);
    rs_access_bits(src);
    rs_access_bytes(src);
    rs_access_bits(src);
    rs_access_bytes(src);
    rs_access_bits(src);
    if (rs_gbit(src, 1) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if (get_skip(src) != k_low_skip) {
        return RS_ERR_FORMAT;
    }
    rs_access_bytes(src);
    return rs_buf_remaining(src) == 0 ? RS_OK : RS_ERR_FORMAT;
}

static const rs_move_layout k_layout = {
    876,
    rs_sizes_c2s_876,
    rs_sizes_s2c_876,
    11,
    44,
    66,
    54,
    -1,
    0,
    0,
    walk_encode,
    walk_decode,
    region_encode,
    region_decode,
    player_encode,
    player_decode
};

static void rs_move_876_init(void) __attribute__((constructor));
static void rs_move_876_init(void) {
    rs_move_register(&k_layout);
}

#ifdef RS_MOVE_TEST
#include <stdio.h>
#include <stdlib.h>


static int fail(const char *msg);

static int player_round(void) {
    rs_buf buf;
    rs_move_step step, got;
    uint8_t raw[128];
    size_t n;

    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = -1;
    step.dir2 = -1;
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (player_encode(NULL, &buf, &step) != RS_OK) {
        return fail("teleport encode");
    }
    n = buf.pos;
    rs_buf_wrap(&buf, raw, n);
    if (player_decode(NULL, &buf, &got) != RS_OK || got.teleport != 1 || got.local_x != 54 ||
        got.local_z != 54 || got.plane != 0 || got.dir1 != -1 || got.dir2 != -1 || got.appearance != 0) {
        return fail("teleport decode");
    }
    memset(&step, 0, sizeof step);
    step.dir1 = 4;
    step.dir2 = -1;
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (player_encode(NULL, &buf, &step) != RS_OK) {
        return fail("east encode");
    }
    n = buf.pos;
    rs_buf_wrap(&buf, raw, n);
    if (player_decode(NULL, &buf, &got) != RS_OK || got.teleport != 0 || got.dir1 != 4 || got.dir2 != -1 ||
        got.appearance != 0) {
        return fail("east decode");
    }
    step.dir1 = 8;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (player_encode(NULL, &buf, &step) != RS_ERR_UNSUPPORTED) {
        return fail("bad dir");
    }
    return 0;
}

static int fail(const char *msg) {
    fprintf(stderr, "876 %s\n", msg);
    return 1;
}

int main(void) {
    rs_session session;
    rs_buf buf;
    rs_walk in, out;
    rs_region rin, rout;
    const rs_move_layout *m;
    const rs_build_info *info;

    m = rs_move_for(876);
    if (!m || m->opcode_walk != 11 || m->opcode_region != 66 || !m->sizes_c2s || !m->sizes_s2c) {
        return fail("register");
    }
    if (!m->player_encode || !m->player_decode) {
        return fail("player");
    }
    info = rs_build_lookup(876);
    if (!info) {
        return fail("build");
    }
    memset(&session, 0, sizeof session);
    rs_session_init_role(&session, info, RS_ROLE_SERVER);
    if (rs_buf_alloc(&buf, 64) != RS_OK) {
        return fail("buf");
    }
    memset(&in, 0, sizeof in);
    in.dest_x = 3200;
    in.dest_z = 40000;
    in.running = 1;
    if (walk_encode(&session, &buf, &in) != RS_OK || buf.pos != 5) {
        return fail("walk encode");
    }
    buf.pos = 0;
    if (walk_decode(&session, &buf, &out) != RS_OK) {
        return fail("walk decode");
    }
    if (out.dest_x != in.dest_x || out.dest_z != in.dest_z || !out.running || out.nsteps != 0) {
        return fail("walk roundtrip");
    }
    in.nsteps = 1;
    if (walk_encode(&session, &buf, &in) != RS_ERR_UNSUPPORTED) {
        return fail("steps");
    }
    rs_buf_reset(&buf);
    memset(&rin, 0, sizeof rin);
    rin.zone_x = 410;
    rin.zone_z = 402;
    rin.build_area = 5;
    rin.force_reload = 1;
    rin.npc_view = 14;
    if (region_encode(&session, &buf, &rin) != RS_OK || buf.pos != 8) {
        return fail("region encode");
    }
    buf.pos = 0;
    if (region_decode(&session, &buf, &rout) != RS_OK) {
        return fail("region decode");
    }
    if (rout.zone_x != 410 || rout.zone_z != 402 || rout.build_area != 5 || !rout.force_reload ||
        rout.npc_view != 14) {
        return fail("region roundtrip");
    }
    rs_buf_free(&buf);
    if (player_round()) {
        return 1;
    }
    return 0;
}
#endif
