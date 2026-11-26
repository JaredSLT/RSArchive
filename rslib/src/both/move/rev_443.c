#include "rslib/both/move.h"
#include "rslib/both/crypto.h"

#include <string.h>

/*
 * 443 PathingEntity packet switch.
 * Walk Keyboard.opKeyboard5: map 99, minimap 80, third 81.
 *   MonotonicClock.opMonotonicClock16 p2_le_add128 X, opMonotonicClock5 p1_neg run,
 *   opMonotonicClock6 p2_le Z, then opMonotonicClock25 dx and opMonotonicClock17 (128-dz).
 *   The length byte is framing. Minimap adds 14 only in that length.
 * Player 29 is GlModel.callde1332aa, then AufDerHautpseite.opkind15420 (8-bit count),
 * ModeratorOptionSpielerUrStundenStummSchalt.opkind19811 (11-bit 2047), RegelverstoMelden.opkind1954.
 * Teleport opkind1872(z, clear, x): 7 local x, 1 mask, 2 plane, 7 local z, 1 clear.
 * Directions NameDesGegenstandsEingeben.opkind18710 match the API (valuekind18765 is x).
 * Mask ext 0x80, appearance 0x40. Length opMonotonicClock8 is g1_add128.
 * opMonotonicClock2 stores each appearance byte minus 128.
 * opkind1842: gender, two signed bytes, 12 slots, 5 colours, 7 anims, name, combat, skill.
 * Region 121 Timer.opTimer4(false) is rs_region_encode_zones. No c2s int[256].
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

static rs_err appearance_logical(rs_buf *dst, const rs_appearance *a) {
    int i;
    rs_err e;
    e = rs_p1(dst, a->gender);
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
        e = rs_p1(dst, 255);
        if (e) {
            return e;
        }
        e = rs_p1(dst, 255);
        if (e) {
            return e;
        }
        e = rs_p2(dst, a->npc_id);
        if (e) {
            return e;
        }
    } else {
        for (i = 0; i < 12; i++) {
            if (i < 7) {
                e = rs_p2(dst, 0x100 + a->styles[i]);
            } else {
                e = rs_p1(dst, 0);
            }
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
    e = rs_p1(dst, a->combat_level);
    if (e) {
        return e;
    }
    return rs_p2(dst, a->skill_level);
}

static rs_err appearance_parse(rs_buf *src, rs_appearance *a) {
    int i;
    uint64_t name;
    memset(a, 0, sizeof(*a));
    a->gender = rs_g1(src);
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
    a->skill_level = rs_g2(src);
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
    e = rs_p1_add128(dst, n);
    if (e) {
        return e;
    }
    for (i = 0; i < n; i++) {
        e = rs_p1(dst, tmp[i] + 128);
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
    n = rs_g1_add128(src);
    e = need(src, (size_t)n);
    if (e) {
        return e;
    }
    for (i = 0; i < n; i++) {
        tmp[i] = (uint8_t)(rs_g1(src) - 128);
    }
    rs_buf_wrap(&body, tmp, (size_t)n);
    e = appearance_parse(&body, a);
    if (e) {
        return e;
    }
    return body.pos == (size_t)n ? RS_OK : RS_ERR_FORMAT;
}

static rs_err walk_encode_443(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    int i;
    rs_err e;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    if (w->nsteps < 0 || w->nsteps > RS_WALK_STEPS) {
        return RS_ERR_RANGE;
    }
    e = rs_p2_le_add128(dst, w->dest_x);
    if (e) {
        return e;
    }
    e = rs_p1_neg(dst, w->running ? 1 : 0);
    if (e) {
        return e;
    }
    e = rs_p2_le(dst, w->dest_z);
    if (e) {
        return e;
    }
    for (i = 0; i < w->nsteps; i++) {
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

static rs_err walk_decode_443(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 5) {
        return RS_ERR_FORMAT;
    }
    memset(w, 0, sizeof(*w));
    w->dest_x = rs_g2_le_add128(src);
    w->running = rs_g1_neg(src) != 0;
    w->dest_z = rs_g2_le(src);
    while (rs_buf_remaining(src) >= 2 && w->nsteps < RS_WALK_STEPS) {
        w->dx[w->nsteps] = (int8_t)rs_g1(src);
        w->dz[w->nsteps] = (int8_t)rs_g1_sub128(src);
        w->nsteps++;
    }
    if (rs_buf_remaining(src) != 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err player_encode_443(const rs_session *s, rs_buf *dst, const rs_move_step *step) {
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
            e = rs_pbit(dst, 2, step->plane & 3);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 7, step->local_z & 127);
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
    e = rs_p1(dst, 0x40);
    if (e) {
        return e;
    }
    return put_appearance(dst, &step->look);
}

static void apply_dir(int *x, int *z, int dir) {
    *x += k_dx[dir];
    *z += k_dz[dir];
}

static rs_err player_decode_443(const rs_session *s, rs_buf *src, rs_move_step *step) {
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
            e = gbits(src, 2, &step->plane);
            if (e) {
                return e;
            }
            e = gbits(src, 7, &step->local_z);
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
    if (mask & 0x80) {
        e = need(src, 1);
        if (e) {
            return e;
        }
        mask |= rs_g1(src) << 8;
    }
    if ((mask & ~0xc0) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if (mask & 0x40) {
        e = get_appearance(src, &step->look);
        if (e) {
            return e;
        }
        step->appearance = 1;
    }
    return rs_buf_remaining(src) == 0 ? RS_OK : RS_ERR_FORMAT;
}

/* Payload after the opcode. -1 is one length byte, -3 was not written.
 * 99 and 80 are walk_encode_443 with nsteps == 0. The client also writes a
 * length byte, and minimap callers add 14 bytes this encoder does not write.
 */
static const int8_t k_c2s_443[256] = {
      8,  -3,  -3,   6,  -1,  -3,  -3,  -3,   6,  -3,  -3,   2,  -3,   6,  -3,  -3,
     -3,  -3,  -3,  -3,  -3,   0,   8,  -3,  -3,  -3,  -3,  -3,   6,   8,  -3,  -3,
     -3,  -3,  -3,  -3,   2,  -3,   6,  -3,   6,  -3,  -3,  -3,  -3,  -3,  12,   6,
      8,  -3,  -1,  -3,  -3,  -3,   4,  -3,  -3,   6,  -3,  -3,  -3,  -3,  -3,  -3,
     12,   1,   4,  -3,  -3,   6,   0,  -3,  -3,   6,   4,  -3,   3,  -3,  12,  -3,
      5,  -1,  -3,  -3,  -3,   6,   0,   2,  -3,   2,   8,  -3,  -3,  -3,  -3,  -3,
     -3,  -3,  -3,   5,  -3,   2,  -3,  -3,  10,  -3,  -3,  -3,  -3,  -3,  -3,  -3,
     -3,   8,  14,  -3,  -3,  -3,  13,  10,   6,  -3,  -3,  -3,  -3,  -3,  -3,  -3,
     -3,  -3,  -3,  -3,  -3,  -1,  -3,  -3,   6,  -3,  -3,  -3,  -3,   4,  -3,  -3,
      8,  12,  10,  16,  -3,   6,  -3,  -3,  -3,   6,   2,  -3,  -3,  -3,   2,   8,
     -3,   6,   4,  -3,  -3,  -3,  -3,   6,  -3,   2,  -3,   8,  -3,  -3,   2,  -3,
     -3,  -3,   8,  -3,  -3,  -3,   8,  -3,  -3,  -3,  -3,  -3,   8,  -3,   9,  -3,
      0,  -3,   0,  14,  -3,  -3,   8,  -3,   8,  -3,   6,  -3,  -3,  -3,   2,   1,
     -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,   6,  -3,   2,  -3,   8,   2,  -3,
      2,  -3,  -3,  -3,  -3,   2,  -3,  -3,  -3,  -3,  -3,  -3,   8,  -3,  -3,  -3,
     -3,  -3,  -3,  14,  -3,   6,  -3,  -3,  -3,  -3,   8,  -3,   6,  -3,  -3,  -3,
};

static const rs_move_layout k_443 = {
    443,
    k_c2s_443,
    NULL,
    99,
    80,
    121,
    29,
    -1,
    0,
    0,
    walk_encode_443,
    walk_decode_443,
    NULL,
    NULL,
    player_encode_443,
    player_decode_443};

void rs_move_443_init(void) {
    rs_move_register(&k_443);
}

static void rs_move_443_ctor(void) __attribute__((constructor));
static void rs_move_443_ctor(void) {
    rs_move_443_init();
}

#ifdef RS_MOVE_TEST
#include <stdio.h>

static int g_fail;

static void expect(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "443 %s\n", msg);
        g_fail = 1;
    }
}

static void fill_look(rs_appearance *a) {
    int i;
    memset(a, 0, sizeof(*a));
    a->gender = 0;
    a->headicon = 0;
    a->styles[0] = 0;
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
    a->skill_level = 15;
}

static int same_look(const rs_appearance *a, const rs_appearance *b) {
    return memcmp(a, b, sizeof(*a)) == 0;
}

int main(void) {
    rs_buf buf;
    rs_move_step step, out;
    rs_walk win, wout;
    uint8_t raw[512];
    const rs_move_layout *m;

    rs_move_443_init();
    m = rs_move_for(443);
    expect(m && m->opcode_walk == 99 && m->opcode_walk_minimap == 80 && m->opcode_player == 29 &&
               m->opcode_region == 121 && m->walk_encode && m->player_encode && !m->region_encode &&
               m->sizes_c2s == k_c2s_443,
           "layout");
    memset(&win, 0, sizeof win);
    win.dest_x = 3222;
    win.dest_z = 3222;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(walk_encode_443(NULL, &buf, &win) == RS_OK && m->sizes_c2s[m->opcode_walk] == (int)buf.pos,
           "c2s walk size");
    expect(m->sizes_c2s[1] == -3, "c2s unknown");

    memset(&win, 0, sizeof win);
    win.dest_x = 3222;
    win.dest_z = 3222;
    win.running = 1;
    win.nsteps = 2;
    win.dx[0] = -1;
    win.dz[0] = 1;
    win.dx[1] = 2;
    win.dz[1] = -2;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(walk_encode_443(NULL, &buf, &win) == RS_OK, "walk enc");
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(walk_decode_443(NULL, &buf, &wout) == RS_OK, "walk dec");
    expect(wout.dest_x == win.dest_x && wout.dest_z == win.dest_z && wout.running == 1 &&
               wout.nsteps == 2 && wout.dx[0] == -1 && wout.dz[0] == 1 && wout.dx[1] == 2 &&
               wout.dz[1] == -2,
           "walk");

    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.plane = 0;
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = -1;
    step.dir2 = -1;
    step.appearance = 1;
    fill_look(&step.look);
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(player_encode_443(NULL, &buf, &step) == RS_OK, "place enc");
    memset(&out, 0, sizeof out);
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(player_decode_443(NULL, &buf, &out) == RS_OK, "place dec");
    expect(out.teleport == 1 && out.plane == 0 && out.local_x == 54 && out.local_z == 54 &&
               out.appearance == 1 && same_look(&step.look, &out.look),
           "place");

    memset(&step, 0, sizeof step);
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = 0;
    step.dir2 = -1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(player_encode_443(NULL, &buf, &step) == RS_OK, "one enc");
    memset(&out, 0, sizeof out);
    out.local_x = 54;
    out.local_z = 54;
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(player_decode_443(NULL, &buf, &out) == RS_OK, "one dec");
    expect(out.teleport == 0 && out.local_x == 53 && out.local_z == 55 && out.dir1 == 0 &&
               out.dir2 == -1 && out.appearance == 0,
           "one");

    memset(&step, 0, sizeof step);
    step.local_x = 53;
    step.local_z = 55;
    step.dir1 = 4;
    step.dir2 = 7;
    rs_buf_wrap(&buf, raw, sizeof raw);
    expect(player_encode_443(NULL, &buf, &step) == RS_OK, "two enc");
    memset(&out, 0, sizeof out);
    out.local_x = 53;
    out.local_z = 55;
    rs_buf_wrap(&buf, raw, buf.pos);
    expect(player_decode_443(NULL, &buf, &out) == RS_OK, "two dec");
    expect(out.local_x == 55 && out.local_z == 54 && out.dir1 == 4 && out.dir2 == 7, "two");
    return g_fail ? 1 : 0;
}
#endif
