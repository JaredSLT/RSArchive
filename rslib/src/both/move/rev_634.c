#include "rslib/both/move.h"
#include "rslib/both/cache.h"

#include <string.h>

/*
 * 634 C79bd777c.opkind14354. Game click Cc6a8f147(35, 5) at
 * Cb6ae220e.valuekind14785. Minimap Cc6a8f147(82, 18) at
 * C7643b519.valuekind11911. The constructor keeps only the opcode.
 * Body is LoadingScreenRelated2.opLoadingScreenRelated234 (p2_le world z),
 * opLoadingScreenRelated269 (p1_add128 running), opLoadingScreenRelated218
 * (p2_add128 world x). No step bytes. Minimap appends 13 client bytes.
 * C42b26f21.opkind14182 indexes server packets: player
 * C2f8bd213.valuekind13907 is 105, rebuild C8d2c3755.valuekind16673 is 19.
 * Rebuild reader is Cd0f64cd0.opkind14622. There is no dense int[256]
 * in the gamepack (C1b4ca309.valuekind17072 is a build-area sentinel).
 * k_sizes_* copies every Cc6a8f147 and C454c974c constructor. Opcodes
 * with no constructor stay -3.
 * Player block is AudioThread.opAudioThread46: VarnBitType.callf97538dc,
 * Cfe056fdd.opkind11624, C91aa4cea.opkind15084, then C5de4f69d.opkind12931 /
 * LoadingRelated2.opLoadingRelated23. Appearance is mask bit 16
 * (C5b4806ed.opkind16055). Length is opLoadingScreenRelated261. opLoadingScreenRelated229
 * reverses the bytes.
 */

static const int k_wire[8] = {5, 6, 7, 3, 4, 0, 1, 2};
static const int k_dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
static const int k_dz[8] = {1, 1, 1, 0, 0, -1, -1, -1};
static const int k_run_dx[16] = {-2, -1, 0, 1, 2, -2, 2, -2, 2, -2, 2, -2, -1, 0, 1, 2};
static const int k_run_dz[16] = {-2, -2, -2, -2, -2, -1, -1, 0, 0, 1, 1, 2, 2, 2, 2, 2};

static rs_err walk_encode(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    rs_err e;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    if (w->nsteps != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    e = rs_p2_le(dst, w->dest_z);
    if (e) {
        return e;
    }
    e = rs_p1_add128(dst, w->running ? 1 : 0);
    if (e) {
        return e;
    }
    return rs_p2_add128(dst, w->dest_x);
}

static rs_err walk_decode(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) != 5) {
        return RS_ERR_FORMAT;
    }
    memset(w, 0, sizeof(*w));
    w->dest_z = rs_g2_le(src);
    w->running = rs_g1_add128(src);
    w->dest_x = rs_g2_add128(src);
    return RS_OK;
}

static rs_err look_logical(rs_buf *b, const rs_appearance *a) {
    int i;
    rs_err e;
    if (a->gender != 0 && a->gender != 1) {
        return RS_ERR_RANGE;
    }
    if (a->headicon < -128 || a->headicon > 127) {
        return RS_ERR_RANGE;
    }
    if (a->npc_id < 0 || a->npc_id > 65535) {
        return RS_ERR_RANGE;
    }
    if (a->combat_level < 0 || a->combat_level > 255 || a->skill_level < 0 || a->skill_level > 255) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < 7; i++) {
        if (a->anims[i] != 0) {
            return RS_ERR_UNSUPPORTED;
        }
        if (a->styles[i] < 0 || a->styles[i] >= 32512) {
            return RS_ERR_RANGE;
        }
    }
    for (i = 0; i < 5; i++) {
        if (a->colours[i] < 0 || a->colours[i] > 255) {
            return RS_ERR_RANGE;
        }
    }
    if (strlen(a->name) > 12) {
        return RS_ERR_RANGE;
    }
    e = rs_p1(b, a->gender & 1);
    if (e) {
        return e;
    }
    e = rs_p1(b, a->headicon);
    if (e) {
        return e;
    }
    e = rs_p1(b, 0xff);
    if (e) {
        return e;
    }
    e = rs_p1(b, 0xff);
    if (e) {
        return e;
    }
    e = rs_p1(b, 0);
    if (e) {
        return e;
    }
    if (a->npc_id > 0) {
        for (i = 0; i < 7; i++) {
            if (a->styles[i] != 0) {
                return RS_ERR_UNSUPPORTED;
            }
        }
        e = rs_p1(b, 0xff);
        if (e) {
            return e;
        }
        e = rs_p1(b, 0xff);
        if (e) {
            return e;
        }
        e = rs_p2(b, a->npc_id);
        if (e) {
            return e;
        }
        e = rs_p1(b, 0);
        if (e) {
            return e;
        }
    } else {
        for (i = 0; i < 7; i++) {
            int id;
            if (a->styles[i] == 0) {
                e = rs_p1(b, 0);
                if (e) {
                    return e;
                }
                continue;
            }
            id = a->styles[i] + 256;
            e = rs_p1(b, id >> 8);
            if (e) {
                return e;
            }
            e = rs_p1(b, id);
            if (e) {
                return e;
            }
        }
        for (i = 0; i < 5; i++) {
            e = rs_p1(b, 0);
            if (e) {
                return e;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        e = rs_p1(b, a->colours[i]);
        if (e) {
            return e;
        }
    }
    e = rs_p2(b, 0);
    if (e) {
        return e;
    }
    e = rs_pjstrnull(b, a->name);
    if (e) {
        return e;
    }
    e = rs_p1(b, a->combat_level);
    if (e) {
        return e;
    }
    e = rs_p1(b, a->skill_level);
    if (e) {
        return e;
    }
    e = rs_p1(b, 255);
    if (e) {
        return e;
    }
    return rs_p1(b, 0);
}

static rs_err look_parse(rs_buf *b, rs_appearance *a) {
    int flags, i, npc;
    rs_err e;
    memset(a, 0, sizeof(*a));
    flags = rs_g1(b);
    if (flags & ~1) {
        return RS_ERR_UNSUPPORTED;
    }
    a->gender = flags & 1;
    a->headicon = rs_g1b(b);
    if (rs_g1b(b) != -1 || rs_g1b(b) != -1) {
        return RS_ERR_UNSUPPORTED;
    }
    if (rs_g1b(b) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    npc = 0;
    for (i = 0; i < 12; i++) {
        int hi = rs_g1(b);
        int id;
        if (hi == 0) {
            continue;
        }
        id = (hi << 8) | rs_g1(b);
        if (i == 0 && id == 65535) {
            a->npc_id = rs_g2(b);
            if (rs_g1(b) != 0) {
                return RS_ERR_UNSUPPORTED;
            }
            npc = 1;
            break;
        }
        if (id >= 32768) {
            return RS_ERR_UNSUPPORTED;
        }
        if (i >= 7) {
            return RS_ERR_UNSUPPORTED;
        }
        a->styles[i] = id - 256;
    }
    (void)npc;
    for (i = 0; i < 5; i++) {
        a->colours[i] = rs_g1(b);
    }
    if (rs_g2(b) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    e = rs_gjstrnull(b, a->name, sizeof a->name);
    if (e) {
        return e;
    }
    a->combat_level = rs_g1(b);
    a->skill_level = rs_g1(b);
    if (rs_g1(b) != 255) {
        return RS_ERR_UNSUPPORTED;
    }
    if (rs_g1(b) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if (rs_buf_remaining(b) != 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err take_look(rs_buf *src, rs_appearance *a) {
    uint8_t raw[96];
    rs_buf logical;
    int len, i;
    len = rs_g1(src);
    if (len < 0 || len > (int)sizeof raw) {
        return RS_ERR_FORMAT;
    }
    for (i = len - 1; i >= 0; i--) {
        raw[i] = (uint8_t)rs_g1(src);
    }
    rs_buf_wrap(&logical, raw, (size_t)len);
    return look_parse(&logical, a);
}

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

/* kind: 0 no-change skip, 1 stationary+mask, 2 one step, 3 two-step, 4 teleport. */
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
        *kind = 4;
        return RS_OK;
    }
    if (step->dir1 == -1 && step->dir2 == -1) {
        *kind = step->appearance ? 1 : 0;
        return RS_OK;
    }
    if (step->dir1 < 0 || step->dir1 > 7) {
        return RS_ERR_RANGE;
    }
    if (step->dir2 == -1) {
        *kind = 2;
        return RS_OK;
    }
    if (step->dir2 < 0 || step->dir2 > 7) {
        return RS_ERR_RANGE;
    }
    *code = run_code(step->dir1, step->dir2);
    if (*code < 0) {
        return RS_ERR_UNSUPPORTED;
    }
    *kind = 3;
    return RS_OK;
}

static rs_err put_move(rs_buf *dst, const rs_move_step *step, int kind, int code) {
    rs_err e;
    int need = step->appearance ? 1 : 0;
    e = rs_pbit(dst, 1, need);
    if (e) {
        return e;
    }
    if (kind == 1) {
        return rs_pbit(dst, 2, 0);
    }
    if (kind == 2) {
        e = rs_pbit(dst, 2, 1);
        if (e) {
            return e;
        }
        return rs_pbit(dst, 3, k_wire[step->dir1]);
    }
    if (kind == 3) {
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
    return rs_pbit(dst, 30, (step->plane << 28) | (step->local_x << 14) | step->local_z);
}

static rs_err player_encode(const rs_session *s, rs_buf *dst, const rs_move_step *step) {
    uint8_t raw[96];
    rs_buf logical;
    int kind = 0, code = 0, i;
    rs_err e;
    (void)s;
    if (!dst || !step) {
        return RS_ERR_ARG;
    }
    e = classify(step, &kind, &code);
    if (e) {
        return e;
    }
    if (step->appearance) {
        rs_buf_wrap(&logical, raw, sizeof raw);
        e = look_logical(&logical, &step->look);
        if (e) {
            return e;
        }
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
    e = put_skip(dst, 2045);
    if (e) {
        return e;
    }
    rs_access_bytes(dst);
    if (!step->appearance) {
        return RS_OK;
    }
    e = rs_p1(dst, 16);
    if (e) {
        return e;
    }
    e = rs_p1(dst, (int)logical.pos);
    if (e) {
        return e;
    }
    for (i = (int)logical.pos - 1; i >= 0; i--) {
        e = rs_p1(dst, raw[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static rs_err take_move(rs_buf *src, rs_move_step *step, int *need) {
    int type, sub, word, code;
    *need = rs_gbit(src, 1);
    type = rs_gbit(src, 2);
    step->dir1 = -1;
    step->dir2 = -1;
    step->teleport = 0;
    if (type == 0) {
        if (!*need) {
            return RS_ERR_UNSUPPORTED;
        }
        return RS_OK;
    }
    if (type == 1) {
        step->dir1 = k_wire[rs_gbit(src, 3)];
        return RS_OK;
    }
    if (type == 2) {
        code = rs_gbit(src, 4);
        if (!split_run(code, &step->dir1, &step->dir2)) {
            return RS_ERR_FORMAT;
        }
        return RS_OK;
    }
    sub = rs_gbit(src, 1);
    if (sub == 0) {
        return RS_ERR_UNSUPPORTED;
    }
    word = rs_gbit(src, 30);
    step->teleport = 1;
    step->plane = (word >> 28) & 3;
    step->local_x = (word >> 14) & 16383;
    step->local_z = word & 16383;
    return RS_OK;
}

static rs_err player_decode(const rs_session *s, rs_buf *src, rs_move_step *step) {
    int bit, need = 0, mask;
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
        e = take_move(src, step, &need);
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
    if (get_skip(src) != 2045) {
        return RS_ERR_FORMAT;
    }
    rs_access_bytes(src);
    if (!need) {
        return rs_buf_remaining(src) == 0 ? RS_OK : RS_ERR_FORMAT;
    }
    mask = rs_g1(src);
    if (mask & 64) {
        mask |= rs_g1(src) << 8;
    }
    if (mask & 16384) {
        mask |= rs_g1(src) << 16;
    }
    if (mask & ~16) {
        return RS_ERR_UNSUPPORTED;
    }
    if (mask & 16) {
        step->appearance = 1;
        e = take_look(src, &step->look);
        if (e) {
            return e;
        }
    }
    return rs_buf_remaining(src) == 0 ? RS_OK : RS_ERR_FORMAT;
}

static int area_tiles(int id) {
    if (id == 0) {
        return 104;
    }
    if (id == 1) {
        return 120;
    }
    if (id == 2) {
        return 136;
    }
    if (id == 3) {
        return 168;
    }
    return -1;
}

static rs_err span_squares(int zone_x, int zone_z, int rad, int *out, int cap, int *n_out) {
    int mx0 = (zone_x - rad) / 8;
    int mx1 = (zone_x + rad) / 8;
    int mz0 = (zone_z - rad) / 8;
    int mz1 = (zone_z + rad) / 8;
    int n = 0;
    int mx, mz;
    for (mx = mx0; mx <= mx1; mx++) {
        for (mz = mz0; mz <= mz1; mz++) {
            if (n >= cap) {
                return RS_ERR_RANGE;
            }
            out[n++] = (mx << 8) | mz;
        }
    }
    *n_out = n;
    return RS_OK;
}

static rs_err write_keys(rs_buf *dst, const rs_region *v, const int *squares, int n) {
    int i, k;
    rs_err e;
    for (i = 0; i < n; i++) {
        const rs_xtea_key *key = rs_cache_find_key_mapsquare(v->cache, squares[i]);
        for (k = 0; k < 4; k++) {
            e = rs_p4(dst, key ? (int)key->key[k] : 0);
            if (e) {
                return e;
            }
        }
    }
    return RS_OK;
}

static rs_err region_encode(const rs_session *s, rs_buf *dst, const rs_region *v) {
    int tiles, squares[64], n;
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    if (v->force_reload != 0 && v->force_reload != 1) {
        return RS_ERR_RANGE;
    }
    tiles = area_tiles(v->build_area);
    if (tiles < 0) {
        return RS_ERR_RANGE;
    }
    e = span_squares(v->zone_x, v->zone_z, tiles >> 4, squares, 64, &n);
    if (e) {
        return e;
    }
    e = rs_p1_sub128(dst, v->build_area);
    if (e) {
        return e;
    }
    e = rs_p2_le_add128(dst, v->zone_z);
    if (e) {
        return e;
    }
    e = rs_p2_le(dst, v->zone_x);
    if (e) {
        return e;
    }
    e = rs_p1_neg(dst, v->force_reload);
    if (e) {
        return e;
    }
    return write_keys(dst, v, squares, n);
}

static rs_err region_decode(const rs_session *s, rs_buf *src, rs_region *v) {
    int area, force, zone_x, zone_z, tiles, squares[64], n, i;
    rs_err e;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 6) {
        return RS_ERR_FORMAT;
    }
    area = rs_g1_sub128(src);
    zone_z = rs_g2_le_add128(src);
    zone_x = rs_g2_le(src);
    force = rs_g1_neg(src);
    if (force != 0 && force != 1) {
        return RS_ERR_RANGE;
    }
    tiles = area_tiles(area);
    if (tiles < 0) {
        return RS_ERR_RANGE;
    }
    e = span_squares(zone_x, zone_z, tiles >> 4, squares, 64, &n);
    if (e) {
        return e;
    }
    if (rs_buf_remaining(src) != (size_t)n * 16) {
        return RS_ERR_FORMAT;
    }
    for (i = 0; i < n * 4; i++) {
        (void)rs_g4(src);
    }
    memset(v, 0, sizeof(*v));
    v->build_area = area;
    v->force_reload = force;
    v->zone_x = zone_x;
    v->zone_z = zone_z;
    return RS_OK;
}

/* k_sizes_c2s_634: packet constructors. Missing opcodes stay -3. */
static const int8_t k_sizes_c2s_634[256] = {
       0,   -1,    6,   -1,    0,    3,   -1,    6,    1,    8,   -1,    8,    3,    2,   -1,    8,  /*   0 */
       7,    8,    3,   -1,   -1,    2,    7,    8,    7,    3,    7,    7,    2,    3,    4,    1,  /*  16 */
       4,    8,   15,    5,    7,    2,    7,    8,   12,   -1,    8,   -1,    3,    7,    2,    4,  /*  32 */
      11,    6,   -1,    3,    4,   -1,   15,    4,    7,    3,    4,    8,    8,   11,    3,    3,  /*  48 */
      -1,    3,   -1,   -1,    2,   -1,    0,    2,   16,   -1,   -1,    3,    4,   -1,   16,    3,  /*  64 */
       7,    3,   18,   -1,   -1,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /*  80 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /*  96 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 112 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 128 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 144 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 160 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 176 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 192 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 208 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 224 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3  /* 240 */
};

/* k_sizes_s2c_634: packet constructors. Missing opcodes stay -3. */
static const int8_t k_sizes_s2c_634[256] = {
      -1,    2,    6,    3,    1,    6,    2,    3,   -2,   -1,   -2,    6,    0,   -2,    2,   16,  /*   0 */
       6,    4,    0,   -2,   -2,   -1,    0,    6,    6,    6,   11,    3,   -2,    7,   -1,   -1,  /*  16 */
       6,    3,   12,    0,   -2,    7,   -1,    8,   -1,    3,   -1,    4,    6,    8,   -1,    6,  /*  32 */
      -2,    4,   -1,   17,   -1,   -2,    4,    0,    2,   10,    2,    3,    0,   -1,    4,    5,  /*  48 */
       2,   -1,    6,    3,    6,   12,   -2,    7,    0,    3,   28,    6,    6,    4,   -1,    3,  /*  64 */
       0,    6,    4,    2,    7,    4,    6,    1,   10,    0,   10,    2,   -1,   -2,   -1,   10,  /*  80 */
       8,    0,   -2,   -1,   -1,   20,    3,    8,    1,   -2,    8,    6,    5,    7,   10,   -1,  /*  96 */
       4,    1,    4,   11,    6,    1,   -2,    6,   -2,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 112 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 128 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 144 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 160 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 176 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 192 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 208 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,  /* 224 */
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3  /* 240 */
};

static const rs_move_layout k_634 = {
    634,
    k_sizes_c2s_634,
    k_sizes_s2c_634,
    35,
    82,
    19,
    105,
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

__attribute__((constructor)) static void rs_move_634_init(void) {
    rs_move_register(&k_634);
}

#ifdef RS_MOVE_TEST
#include <stdio.h>

static int fail(const char *msg) {
    fprintf(stderr, "634 %s\n", msg);
    return 1;
}

int main(void) {
    rs_buf buf;
    rs_walk in, out;
    rs_move_step step, got;
    rs_region region, region_out;
    const rs_move_layout *m;
    uint8_t raw[512];
    static const uint8_t expect[] = {0x96, 0x0c, 0x80, 0x0c, 0x16};
    static const uint8_t region_head[] = {0x80, 0x12, 0x01, 0x92, 0x01, 0x00};
    int i;

    rs_move_634_init();
    m = rs_move_for(634);
    if (!m || m->opcode_walk != 35 || m->opcode_walk_minimap != 82 || m->opcode_region != 19 ||
        m->opcode_player != 105 || m->opcode_player_index != -1 || m->walk_encode == NULL ||
        m->region_encode == NULL || m->player_encode == NULL || m->sizes_c2s == NULL ||
        m->sizes_s2c == NULL || m->sizes_c2s[35] != 5 || m->sizes_c2s[82] != 18 ||
        m->sizes_s2c[19] != -2 || m->sizes_s2c[105] != -2) {
        return fail("layout");
    }
    memset(&in, 0, sizeof in);
    in.dest_x = 3222;
    in.dest_z = 3222;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (walk_encode(NULL, &buf, &in) != RS_OK || buf.pos != sizeof expect ||
        memcmp(raw, expect, sizeof expect) != 0) {
        return fail("walk bytes");
    }
    rs_buf_wrap(&buf, raw, sizeof expect);
    if (walk_decode(NULL, &buf, &out) != RS_OK || out.dest_x != 3222 || out.dest_z != 3222 ||
        out.running != 0 || out.nsteps != 0) {
        return fail("walk decode");
    }
    in.nsteps = 1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (walk_encode(NULL, &buf, &in) != RS_ERR_UNSUPPORTED) {
        return fail("steps");
    }
    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = -1;
    step.dir2 = -1;
    step.appearance = 1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (player_encode(NULL, &buf, &step) != RS_OK || buf.pos == 0) {
        return fail("player encode");
    }
    {
        size_t n = buf.pos;
        rs_buf_wrap(&buf, raw, n);
        if (player_decode(NULL, &buf, &got) != RS_OK || got.teleport != 1 || got.local_x != 54 ||
            got.local_z != 54 || got.plane != 0 || got.dir1 != -1 || got.dir2 != -1 ||
            got.appearance != 1 || got.look.gender != 0 || got.look.name[0] != 0) {
            return fail("player decode");
        }
    }
    memset(&step, 0, sizeof step);
    step.dir1 = 1;
    step.dir2 = -1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (player_encode(NULL, &buf, &step) != RS_OK) {
        return fail("step encode");
    }
    {
        size_t n = buf.pos;
        rs_buf_wrap(&buf, raw, n);
        if (player_decode(NULL, &buf, &got) != RS_OK || got.teleport != 0 || got.dir1 != 1 ||
            got.dir2 != -1 || got.appearance != 0) {
            return fail("step decode");
        }
    }
    step.dir1 = 0;
    step.dir2 = 2;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (player_encode(NULL, &buf, &step) != RS_OK) {
        return fail("run encode");
    }
    {
        size_t n = buf.pos;
        rs_buf_wrap(&buf, raw, n);
        if (player_decode(NULL, &buf, &got) != RS_OK || got.dir1 != 0 || got.dir2 != 2) {
            return fail("run decode");
        }
    }
    step.dir1 = 1;
    step.dir2 = 4;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (player_encode(NULL, &buf, &step) != RS_ERR_UNSUPPORTED) {
        return fail("run gap");
    }
    step.look.gender = 1;
    memcpy(step.look.name, "a", 2);
    step.look.styles[0] = 1;
    step.look.colours[0] = 2;
    step.look.combat_level = 3;
    step.look.skill_level = 1;
    step.look.headicon = 4;
    step.teleport = 1;
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = -1;
    step.dir2 = -1;
    step.appearance = 1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (player_encode(NULL, &buf, &step) != RS_OK) {
        return fail("look encode");
    }
    {
        size_t n = buf.pos;
        rs_buf_wrap(&buf, raw, n);
        if (player_decode(NULL, &buf, &got) != RS_OK || got.look.gender != 1 ||
            strcmp(got.look.name, "a") != 0 || got.look.styles[0] != 1 || got.look.colours[0] != 2 ||
            got.look.combat_level != 3 || got.look.skill_level != 1 || got.look.headicon != 4) {
            return fail("look decode");
        }
    }
    memset(&region, 0, sizeof region);
    region.zone_x = 402;
    region.zone_z = 402;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (region_encode(NULL, &buf, &region) != RS_OK || buf.pos != 150 ||
        memcmp(raw, region_head, sizeof region_head) != 0) {
        return fail("region bytes");
    }
    for (i = 6; i < 150; i++) {
        if (raw[i] != 0) {
            return fail("region keys");
        }
    }
    rs_buf_wrap(&buf, raw, 150);
    if (region_decode(NULL, &buf, &region_out) != RS_OK || region_out.zone_x != 402 ||
        region_out.zone_z != 402 || region_out.build_area != 0 || region_out.force_reload != 0) {
        return fail("region decode");
    }
    region.build_area = 4;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (region_encode(NULL, &buf, &region) != RS_ERR_RANGE) {
        return fail("region area");
    }
    return 0;
}
#endif
