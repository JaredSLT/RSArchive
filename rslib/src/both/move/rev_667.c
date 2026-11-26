#include "rslib/both/move.h"

#include <string.h>

/*
 * 667 deob/LIIIYUf.opLIIIYUf2. Game click HitmarkType.valueHitmarkType7
 * is StockMarketOffer(12, 5). Minimap PingProvider.valuePingProvider9 is
 * StockMarketOffer(83, 18). Body is LoadingScreenRelated2.opLoadingScreenRelated2103
 * twice (p2 le-add128 dest x, then dest z) then opLoadingScreenRelated2122 (p1 run).
 * No step bytes. The minimap's extra 13 bytes are client-only.
 * Region opcode ColFfff.valuekind201810 is C250138bd(43, -2). The zones
 * encoder already lives in rs_region_encode_zones. Player opcode
 * SoftwareToolkitFactory.valueSoftwareToolkitFactory23 is C250138bd(69, -2)
 * and reads the 2011 high/low-res block, not a 317 terminator.
 */

static rs_err walk_encode_667(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    rs_err e;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    if (w->nsteps != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    e = rs_p2_le_add128(dst, w->dest_x);
    if (e) {
        return e;
    }
    e = rs_p2_le_add128(dst, w->dest_z);
    if (e) {
        return e;
    }
    return rs_p1(dst, w->running ? 1 : 0);
}

static rs_err walk_decode_667(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) != 5) {
        return RS_ERR_FORMAT;
    }
    memset(w, 0, sizeof(*w));
    w->dest_x = rs_g2_le_add128(src);
    w->dest_z = rs_g2_le_add128(src);
    w->running = rs_g1(src);
    return RS_OK;
}

static const int k_rs3 = 0;

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

static const rs_move_layout k_667 = {
    667,
    NULL,
    NULL,
    12,
    83,
    43,
    69,
    -1,
    0,
    0,
    walk_encode_667,
    walk_decode_667,
    NULL,
    NULL,
    player_encode,
    player_decode
};

__attribute__((constructor)) static void rs_move_667_init(void) {
    rs_move_register(&k_667);
}

#ifdef RS_MOVE_TEST
#include <stdio.h>


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
    fprintf(stderr, "667 %s\n", msg);
    return 1;
}

int main(void) {
    rs_buf buf;
    rs_walk in, out;
    const rs_move_layout *m;
    uint8_t raw[8];
    /* 3222,3222 run: le-add128 0x0C96 is 0x16,0x0C twice, then p1(1). */
    static const uint8_t expect[] = {0x16, 0x0c, 0x16, 0x0c, 0x01};

    m = rs_move_for(667);
    if (!m || m->opcode_walk != 12 || m->opcode_walk_minimap != 83 || m->opcode_region != 43 ||
        m->opcode_player != 69 || m->walk_encode == NULL || m->region_encode != NULL ||
        m->player_encode == NULL || m->player_decode == NULL || m->sizes_c2s != NULL || m->sizes_s2c != NULL) {
        return fail("layout");
    }
    memset(&in, 0, sizeof in);
    in.dest_x = 3222;
    in.dest_z = 3222;
    in.running = 1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (walk_encode_667(NULL, &buf, &in) != RS_OK || buf.pos != sizeof expect ||
        memcmp(raw, expect, sizeof expect) != 0) {
        return fail("walk bytes");
    }
    rs_buf_wrap(&buf, raw, sizeof expect);
    if (walk_decode_667(NULL, &buf, &out) != RS_OK || out.dest_x != 3222 || out.dest_z != 3222 ||
        out.running != 1 || out.nsteps != 0) {
        return fail("walk decode");
    }
    in.nsteps = 1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (walk_encode_667(NULL, &buf, &in) != RS_ERR_UNSUPPORTED) {
        return fail("steps");
    }
    if (player_round()) {
        return 1;
    }
    return 0;
}
#endif
