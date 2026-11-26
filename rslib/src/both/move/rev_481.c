#include "rslib/both/move.h"
#include "rslib/both/cache.h"
#include "rslib/both/crypto.h"

#include <string.h>

/*
 * 481 SpotAnim switch. Player ~opcode == -255 (254) is
 * UnerwarteteAntwortVomAnmeldeServer.opkind2683: C105f7f5d.opkind3425,
 * LongNode.opLongNode23 (8-bit count), Ccc509c66.opkind3527 (11-bit 2047),
 * World.opWorld78. Teleport opDirectByteArray22(z, clear, x): 7 local x,
 * 1 mask, 7 local z, 2 plane, 1 clear. opDirectByteArray26 matches the API
 * (value67 is x, value74 is z).
 * Mask ext bit 0x02, appearance bit 0x04. Length opkind33424 is g1_sub128.
 * call56371ce8 reverses the bytes. MapMarker.opMapMarker20: flag byte
 * (bit 0 gender; bit 1 and bit 2 add unread blocks), two signed icons,
 * 12 slots, 5 colours, 7 anims, name, combat. skill_level is not in that body.
 * Walk GameShell.opGameShell27 opcodes 198 / 249 / 95: opkind33413 p2_le_add128 Z,
 * opkind33423 p2_le X, opkind33422 p1_add128 dx and dz, opkind33446 run.
 * The length byte is framing. Sprite.opSprite34 adds 14 minimap bytes this
 * encoder does not write. Rebuild SieBefindenSichEinemMitgliederGebiet.opkind2783(false) opcode 126:
 * p2_le local x, LE keys, g1_neg plane, p2_le_add128 zone x,
 * p2_le_add128 local z, g2 zone z. Instanced opcode 187 is not this packet.
 * valuekind3423 is the existing s2c size table, not a c2s table.
 */

static const int k_dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
static const int k_dz[8] = {1, 1, 1, 0, 0, -1, -1, -1};
static const int k_anim[7] = {0x328, 0x337, 0x333, 0x334, 0x335, 0x336, 0x338};

static rs_err gbits(rs_buf *b, int n, int *out) {
    if (!b || n < 1 || b->bit_pos + (size_t)n > b->cap * 8) {
        return RS_ERR_FORMAT;
    }
    *out = rs_gbit(b, n);
    return RS_OK;
}

static rs_err need(rs_buf *b, size_t n) {
    if (!b || rs_buf_remaining(b) < n) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static int map_squares(int zone_x, int zone_z, int *out, int cap) {
    int mx0 = (zone_x - 6) / 8;
    int mx1 = (zone_x + 6) / 8;
    int mz0 = (zone_z - 6) / 8;
    int mz1 = (zone_z + 6) / 8;
    int skip = ((zone_x / 8 == 48 || zone_x / 8 == 49) && zone_z / 8 == 48) ||
               (zone_x / 8 == 48 && zone_z / 8 == 148);
    int n = 0;
    int mx, mz;
    for (mx = mx0; mx <= mx1; mx++) {
        for (mz = mz0; mz <= mz1; mz++) {
            if (skip && (mz == 49 || mz == 149 || mz == 147 || mx == 50 || (mx == 49 && mz == 47))) {
                continue;
            }
            if (n >= cap) {
                return -1;
            }
            out[n++] = (mx << 8) | mz;
        }
    }
    return n;
}

static rs_err write_keys(rs_buf *dst, const rs_region *v, const int *squares, int n) {
    int i, k;
    rs_err e;
    for (i = 0; i < n; i++) {
        const rs_xtea_key *key = rs_cache_find_key_mapsquare(v->cache, squares[i]);
        for (k = 0; k < 4; k++) {
            e = rs_ip4(dst, key ? (int)key->key[k] : 0);
            if (e) {
                return e;
            }
        }
    }
    return RS_OK;
}

static rs_err region_encode_481(const rs_session *s, rs_buf *dst, const rs_region *v) {
    int squares[16], n, lx, lz;
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    n = map_squares(v->zone_x, v->zone_z, squares, 16);
    if (n < 0) {
        return RS_ERR_RANGE;
    }
    lx = v->local_x ? v->local_x : 52;
    lz = v->local_z ? v->local_z : 52;
    e = rs_p2_le(dst, lx);
    if (e) {
        return e;
    }
    e = write_keys(dst, v, squares, n);
    if (e) {
        return e;
    }
    e = rs_p1_neg(dst, v->plane & 3);
    if (e) {
        return e;
    }
    e = rs_p2_le_add128(dst, v->zone_x);
    if (e) {
        return e;
    }
    e = rs_p2_le_add128(dst, lz);
    if (e) {
        return e;
    }
    return rs_p2(dst, v->zone_z);
}

static rs_err region_decode_481(const rs_session *s, rs_buf *src, rs_region *v) {
    int squares[16], n, expect, i;
    size_t rest;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 9) {
        return RS_ERR_FORMAT;
    }
    memset(v, 0, sizeof(*v));
    v->local_x = rs_g2_le(src);
    rest = rs_buf_remaining(src);
    if (rest < 7 || ((rest - 7) % 16) != 0) {
        return RS_ERR_FORMAT;
    }
    n = (int)((rest - 7) / 16);
    for (i = 0; i < n * 4; i++) {
        (void)rs_ig4(src);
    }
    v->plane = rs_g1_neg(src);
    v->zone_x = rs_g2_le_add128(src);
    v->local_z = rs_g2_le_add128(src);
    v->zone_z = rs_g2(src);
    expect = map_squares(v->zone_x, v->zone_z, squares, 16);
    if (expect < 0 || expect != n) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err appearance_logical(rs_buf *dst, const rs_appearance *a) {
    int i;
    rs_err e;
    if (a->gender != 0 && a->gender != 1) {
        return RS_ERR_RANGE;
    }
    e = rs_p1(dst, a->gender & 1);
    if (e) {
        return e;
    }
    e = rs_p1(dst, a->headicon);
    if (e) {
        return e;
    }
    e = rs_p1(dst, -1);
    if (e) {
        return e;
    }
    if (a->npc_id > 0) {
        e = rs_p2(dst, 65535);
        if (e) {
            return e;
        }
        e = rs_p2(dst, a->npc_id);
        if (e) {
            return e;
        }
    } else {
        for (i = 0; i < 12; i++) {
            e = (i < 7) ? rs_p2(dst, 0x100 + a->styles[i]) : rs_p1(dst, 0);
            if (e) {
                return e;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        e = rs_p1(dst, a->colours[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < 7; i++) {
        int anim = a->anims[i];
        if (anim == 0) {
            anim = k_anim[i];
        } else if (anim < 0) {
            anim = 65535;
        }
        e = rs_p2(dst, anim);
        if (e) {
            return e;
        }
    }
    e = rs_p8(dst, rs_base37_encode(a->name));
    if (e) {
        return e;
    }
    return rs_p1(dst, a->combat_level);
}

static rs_err appearance_parse(rs_buf *src, rs_appearance *a) {
    int i, flags;
    uint64_t name;
    memset(a, 0, sizeof(*a));
    flags = rs_g1(src);
    if ((flags & ~1) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    a->gender = flags & 1;
    a->headicon = (int8_t)rs_g1(src);
    (void)(int8_t)rs_g1(src);
    for (i = 0; i < 12; i++) {
        int hi = rs_g1(src);
        int v;
        if (hi == 0) {
            continue;
        }
        v = (hi << 8) | rs_g1(src);
        if (i == 0 && v == 65535) {
            a->npc_id = rs_g2(src);
            break;
        }
        if (i < 7) {
            a->styles[i] = v - 0x100;
        }
    }
    for (i = 0; i < 5; i++) {
        a->colours[i] = rs_g1(src);
    }
    for (i = 0; i < 7; i++) {
        int anim = rs_g2(src);
        if (anim == 65535) {
            a->anims[i] = -1;
        } else if (anim == k_anim[i]) {
            a->anims[i] = 0;
        } else {
            a->anims[i] = anim;
        }
    }
    name = rs_g8(src);
    if (name == 0) {
        a->name[0] = 0;
    } else {
        rs_base37_decode(name, a->name);
    }
    a->combat_level = rs_g1(src);
    a->skill_level = 0;
    return RS_OK;
}

static rs_err put_appearance(rs_buf *dst, const rs_appearance *a) {
    uint8_t tmp[160];
    rs_buf body;
    rs_err e;
    int n, i;
    if (a->headicon < -128 || a->headicon > 127) {
        return RS_ERR_RANGE;
    }
    rs_buf_wrap(&body, tmp, sizeof tmp);
    e = appearance_logical(&body, a);
    if (e) {
        return e;
    }
    n = (int)body.pos;
    e = rs_p1_sub128(dst, n);
    if (e) {
        return e;
    }
    for (i = n - 1; i >= 0; i--) {
        e = rs_p1(dst, tmp[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static rs_err get_appearance(rs_buf *src, rs_appearance *a) {
    uint8_t tmp[256];
    rs_buf body;
    int n, i;
    rs_err e;
    e = need(src, 1);
    if (e) {
        return e;
    }
    n = rs_g1_sub128(src);
    e = need(src, (size_t)n);
    if (e) {
        return e;
    }
    for (i = n - 1; i >= 0; i--) {
        tmp[i] = (uint8_t)rs_g1(src);
    }
    rs_buf_wrap(&body, tmp, (size_t)n);
    e = appearance_parse(&body, a);
    if (e) {
        return e;
    }
    return body.pos == (size_t)n ? RS_OK : RS_ERR_FORMAT;
}

static rs_err walk_encode_481(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    int i;
    rs_err e;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    if (w->nsteps < 0 || w->nsteps > RS_WALK_STEPS) {
        return RS_ERR_RANGE;
    }
    e = rs_p2_le_add128(dst, w->dest_z);
    if (e) {
        return e;
    }
    e = rs_p2_le(dst, w->dest_x);
    if (e) {
        return e;
    }
    for (i = 0; i < w->nsteps; i++) {
        e = rs_p1_add128(dst, w->dx[i]);
        if (e) {
            return e;
        }
        e = rs_p1_add128(dst, w->dz[i]);
        if (e) {
            return e;
        }
    }
    return rs_p1(dst, w->running ? 1 : 0);
}

static rs_err walk_decode_481(const rs_session *s, rs_buf *src, rs_walk *w) {
    int n;
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 5 || ((rs_buf_remaining(src) - 5) % 2) != 0) {
        return RS_ERR_FORMAT;
    }
    n = (int)(rs_buf_remaining(src) - 5) / 2;
    if (n > RS_WALK_STEPS) {
        return RS_ERR_RANGE;
    }
    memset(w, 0, sizeof(*w));
    w->dest_z = rs_g2_le_add128(src);
    w->dest_x = rs_g2_le(src);
    w->nsteps = n;
    for (n = 0; n < w->nsteps; n++) {
        w->dx[n] = (int8_t)rs_g1_add128(src);
        w->dz[n] = (int8_t)rs_g1_add128(src);
    }
    w->running = rs_g1(src) != 0;
    return RS_OK;
}

static void apply_dir(int *x, int *z, int dir) {
    *x += k_dx[dir];
    *z += k_dz[dir];
}

static rs_err player_encode_481(const rs_session *s, rs_buf *dst, const rs_move_step *step) {
    int walk, run, mask, queued;
    rs_err e;
    (void)s;
    if (!dst || !step) {
        return RS_ERR_ARG;
    }
    walk = step->dir1 >= 0 && step->dir1 <= 7;
    run = step->dir2 >= 0 && step->dir2 <= 7;
    if (step->teleport) {
        if (step->plane < 0 || step->plane > 3 || step->local_x < 0 || step->local_x > 127 ||
            step->local_z < 0 || step->local_z > 127) {
            return RS_ERR_RANGE;
        }
    } else if (run && !walk) {
        return RS_ERR_UNSUPPORTED;
    } else if (!walk && (step->dir1 >= 0 || step->dir2 >= 0)) {
        return RS_ERR_UNSUPPORTED;
    }
    if (step->appearance && step->look.skill_level != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    mask = step->appearance ? 1 : 0;
    queued = mask;
    rs_access_bits(dst);
    if (!step->teleport && !walk && !mask) {
        e = rs_pbit(dst, 1, 0);
    } else {
        e = rs_pbit(dst, 1, 1);
        if (e) {
            return e;
        }
        if (step->teleport) {
            e = rs_pbit(dst, 2, 3);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 7, step->local_x & 127);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, mask);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 7, step->local_z & 127);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 2, step->plane & 3);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, 1);
        } else if (run) {
            e = rs_pbit(dst, 2, 2);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 3, step->dir1);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 3, step->dir2);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, mask);
        } else if (walk) {
            e = rs_pbit(dst, 2, 1);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 3, step->dir1);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, mask);
        } else {
            e = rs_pbit(dst, 2, 0);
            queued = 1;
        }
    }
    if (e) {
        return e;
    }
    e = rs_pbit(dst, 8, 0);
    if (e) {
        return e;
    }
    e = rs_pbit(dst, 11, 2047);
    if (e) {
        return e;
    }
    rs_access_bytes(dst);
    if (!queued) {
        return RS_OK;
    }
    if (!step->appearance) {
        return rs_p1(dst, 0);
    }
    e = rs_p1(dst, 0x04);
    if (e) {
        return e;
    }
    return put_appearance(dst, &step->look);
}

static rs_err player_decode_481(const rs_session *s, rs_buf *src, rs_move_step *step) {
    int flag, type, upd, count, id, mask, prev_x, prev_z, prev_plane;
    rs_err e;
    (void)s;
    if (!src || !step) {
        return RS_ERR_ARG;
    }
    prev_x = step->local_x;
    prev_z = step->local_z;
    prev_plane = step->plane;
    memset(step, 0, sizeof(*step));
    step->dir1 = -1;
    step->dir2 = -1;
    step->local_x = prev_x;
    step->local_z = prev_z;
    step->plane = prev_plane;
    rs_access_bits(src);
    e = gbits(src, 1, &flag);
    if (e) {
        return e;
    }
    upd = 0;
    if (flag) {
        e = gbits(src, 2, &type);
        if (e) {
            return e;
        }
        if (type == 0) {
            upd = 1;
        } else if (type == 1) {
            e = gbits(src, 3, &step->dir1);
            if (e) {
                return e;
            }
            e = gbits(src, 1, &upd);
            if (!e) {
                apply_dir(&step->local_x, &step->local_z, step->dir1);
            }
        } else if (type == 2) {
            e = gbits(src, 3, &step->dir1);
            if (e) {
                return e;
            }
            e = gbits(src, 3, &step->dir2);
            if (e) {
                return e;
            }
            e = gbits(src, 1, &upd);
            if (!e) {
                apply_dir(&step->local_x, &step->local_z, step->dir1);
                apply_dir(&step->local_x, &step->local_z, step->dir2);
            }
        } else {
            int clear;
            step->teleport = 1;
            e = gbits(src, 7, &step->local_x);
            if (e) {
                return e;
            }
            e = gbits(src, 1, &upd);
            if (e) {
                return e;
            }
            e = gbits(src, 7, &step->local_z);
            if (e) {
                return e;
            }
            e = gbits(src, 2, &step->plane);
            if (e) {
                return e;
            }
            e = gbits(src, 1, &clear);
            (void)clear;
        }
        if (e) {
            return e;
        }
    }
    e = gbits(src, 8, &count);
    if (e) {
        return e;
    }
    if (count != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    e = gbits(src, 11, &id);
    if (e) {
        return e;
    }
    if (id != 2047) {
        return RS_ERR_UNSUPPORTED;
    }
    rs_access_bytes(src);
    if (!upd) {
        return rs_buf_remaining(src) == 0 ? RS_OK : RS_ERR_FORMAT;
    }
    e = need(src, 1);
    if (e) {
        return e;
    }
    mask = rs_g1(src);
    if (mask & 0x02) {
        e = need(src, 1);
        if (e) {
            return e;
        }
        mask |= rs_g1(src) << 8;
    }
    if ((mask & ~0x06) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if (mask & 0x04) {
        e = get_appearance(src, &step->look);
        if (e) {
            return e;
        }
        step->appearance = 1;
    }
    return rs_buf_remaining(src) == 0 ? RS_OK : RS_ERR_FORMAT;
}

/* Payload after the opcode. GameShell.opGameShell27. -1 is one length byte,
 * -3 was not written. 198, 249, and 95 are length-prefixed walks. Minimap
 * adds 14 bytes after the shared body; this encoder does not write them.
 */
static const int8_t k_c2s_481[256] = {
     10,  -3,   2,  -3,  -3,  -3,  -3,  -3,   8,  -3,  -3,   6,   2,   2,  -3,  -3,
     -3,  -3,  -3,  -3,  -3,   8,  -3,  -3,  -3,  -3,   6,  -3,  14,   6,   1,  -3,
      2,  -3,  -3,  -3,  -3,  -3,  -3,  -3,   4,  -3,   8,  -3,  -3,  -3,   8,   8,
     12,  -3,  -3,  -3,  -3,   8,   6,  -3,  -3,  -3,  -3,  -3,   6,   4,  -3,  -3,
      6,   2,  -1,  -3,  -3,   8,  -1,  -3,  -3,  -3,   2,  -3,  -3,   6,  -3,   0,
     -3,  -3,  -3,  -3,  -3,   2,  -3,   2,  -3,  -3,   4,   0,   6,  -3,  -3,  -1,
     -3,  -3,  -3,  -3,   3,   8,  -3,  -3,  14,   6,   8,  -1,  -3,  -3,   8,  -3,
     -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,
     -3,  -3,   6,  -3,   6,   2,   8,  -3,  -3,  -3,  -3,  -3,   8,  -3,  -3,  -3,
     -3,  -3,   0,   6,  -1,  -3,  -3,  -3,  -3,  10,   6,   6,  -3,  -3,   6,   2,
     -3,  16,  -3,   2,  -3,  -3,  -3,   2,  12,  -3,   6,   2,   8,   6,  -3,  -3,
      2,  -3,  -3,   6,   6,  -3,  -3,   4,  -3,   8,  -3,  -3,  -3,  -3,   8,   8,
     -3,  10,  -3,  12,  -1,  -3,  -1,  -3,  -3,   9,   8,  20,  -3,  -3,  -3,  -3,
     -3,  -3,  -3,  -3,  -3,   4,   2,  -3,   6,  -3,  -3,   0,  -3,   6,  -3,   2,
     -3,  -3,  -3,  -3,  -3,   9,  -3,   2,  14,  -3,  -3,  -3,   8,  -3,  -3,  -3,
     -3,   8,   4,  -3,  -3,  -3,  12,  -3,  -1,  -1,  -3,  -3,  -3,  -3,  -3,  -3,
};

static const rs_move_layout k_481 = {
    481, k_c2s_481, NULL, 198, 249, 126, 254, -1, 0, 0, walk_encode_481, walk_decode_481,
    region_encode_481, region_decode_481, player_encode_481, player_decode_481};

void rs_move_481_init(void) {
    rs_move_register(&k_481);
}

static void rs_move_481_ctor(void) __attribute__((constructor));
static void rs_move_481_ctor(void) {
    rs_move_481_init();
}

#ifdef RS_MOVE_TEST
#include <stdio.h>

static int g_fail;

static void expect(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "481 %s\n", msg);
        g_fail = 1;
    }
}

static void fill_look(rs_appearance *a) {
    int i;
    memset(a, 0, sizeof(*a));
    a->styles[1] = 10;
    a->styles[2] = 18;
    a->styles[3] = 26;
    a->styles[4] = 33;
    a->styles[5] = 36;
    a->styles[6] = 42;
    for (i = 0; i < 5; i++) {
        a->colours[i] = i + 1;
    }
    memcpy(a->name, "zezima", 7);
    a->combat_level = 3;
}

int main(void) {
    rs_buf buf;
    rs_move_step step, out;
    rs_walk win, wout;
    rs_region region, rout;
    uint8_t raw[512];
    const rs_move_layout *m;

    rs_move_481_init();
    m = rs_move_for(481);
    expect(m && m->opcode_walk == 198 && m->opcode_walk_minimap == 249 && m->opcode_player == 254 &&
               m->opcode_region == 126 && m->walk_encode && m->region_encode && m->player_encode &&
               m->sizes_c2s == k_c2s_481,
           "layout");
    memset(&win, 0, sizeof win);
    win.dest_x = 3222;
    win.dest_z = 3222;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(walk_encode_481(NULL, &buf, &win) == RS_OK && buf.pos == 5 && m->sizes_c2s[198] == -1 &&
               m->sizes_c2s[249] == -1 && m->sizes_c2s[95] == -1,
           "c2s walk size");
    expect(m->sizes_c2s[1] == -3, "c2s unknown");
    memset(&region, 0, sizeof region);
    region.zone_x = 402;
    region.zone_z = 402;
    region.local_x = 54;
    region.local_z = 54;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(region_encode_481(NULL, &buf, &region) == RS_OK, "region enc");
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(region_decode_481(NULL, &buf, &rout) == RS_OK && rout.zone_x == 402 && rout.zone_z == 402 &&
               rout.local_x == 54 && rout.local_z == 54,
           "region");
    memset(&win, 0, sizeof win);
    win.dest_x = 3222;
    win.dest_z = 3223;
    win.running = 1;
    win.nsteps = 2;
    win.dx[0] = -1;
    win.dz[0] = 1;
    win.dx[1] = 2;
    win.dz[1] = -2;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(walk_encode_481(NULL, &buf, &win) == RS_OK, "walk enc");
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(walk_decode_481(NULL, &buf, &wout) == RS_OK && wout.dest_x == 3222 && wout.dest_z == 3223 &&
               wout.running == 1 && wout.nsteps == 2 && wout.dx[0] == -1 && wout.dz[0] == 1 &&
               wout.dx[1] == 2 && wout.dz[1] == -2,
           "walk");
    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = -1;
    step.dir2 = -1;
    step.appearance = 1;
    fill_look(&step.look);
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(player_encode_481(NULL, &buf, &step) == RS_OK, "place enc");
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(player_decode_481(NULL, &buf, &out) == RS_OK && out.teleport == 1 && out.local_x == 54 &&
               out.local_z == 54 && out.appearance == 1 && memcmp(&step.look, &out.look, sizeof out.look) == 0,
           "place");
    memset(&step, 0, sizeof step);
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = 0;
    step.dir2 = -1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(player_encode_481(NULL, &buf, &step) == RS_OK, "one enc");
    memset(&out, 0, sizeof out);
    out.local_x = 54;
    out.local_z = 54;
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(player_decode_481(NULL, &buf, &out) == RS_OK && out.local_x == 53 && out.local_z == 55 &&
               out.dir1 == 0,
           "one");
    memset(&step, 0, sizeof step);
    step.local_x = 53;
    step.local_z = 55;
    step.dir1 = 4;
    step.dir2 = 7;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(player_encode_481(NULL, &buf, &step) == RS_OK, "two enc");
    memset(&out, 0, sizeof out);
    out.local_x = 53;
    out.local_z = 55;
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(player_decode_481(NULL, &buf, &out) == RS_OK && out.local_x == 55 && out.local_z == 54 &&
               out.dir1 == 4 && out.dir2 == 7,
           "two");
    return g_fail ? 1 : 0;
}
#endif
