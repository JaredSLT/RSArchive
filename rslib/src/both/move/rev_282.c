#include "rslib/both/move.h"

#include <string.h>

/* Client.opClient8 opcodes 207/86/138. Same payload as 254 tryMove, not 317. */
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

/* Client.read opcode 231: g2 zone x, g2 zone z. Base is (zone - 6) * 8. */
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

static rs_err gbits(rs_buf *b, int n, int *out) {
    size_t end;
    if (!b || !out || n < 0 || n > 32) {
        return RS_ERR_ARG;
    }
    end = b->bit_pos + (size_t)n;
    if ((end + 7) / 8 > b->cap) {
        return RS_ERR_NEED_MORE;
    }
    *out = rs_gbit(b, n);
    return RS_OK;
}

static rs_err need(rs_buf *b, size_t n) {
    if (!b || rs_buf_remaining(b) < n) {
        return RS_ERR_NEED_MORE;
    }
    return RS_OK;
}

/* PlayerEntity.opkind111. 12 slots, then colours, 7 anims, name, combat, skill. */
static rs_err appearance_write(rs_buf *dst, const rs_appearance *a) {
    int i;
    rs_err e;
    if (a->gender < 0 || a->gender > 255 || a->headicon < 0 || a->headicon > 255) {
        return RS_ERR_RANGE;
    }
    if (a->combat_level < 0 || a->combat_level > 255 || a->skill_level < 0 || a->skill_level > 65535) {
        return RS_ERR_RANGE;
    }
    if (a->npc_id < 0 || a->npc_id > 65535) {
        return RS_ERR_RANGE;
    }
    e = rs_p1(dst, a->gender);
    if (e) {
        return e;
    }
    e = rs_p1(dst, a->headicon);
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
            int id = (i < 7) ? a->styles[i] : 0;
            if (id < 0 || id > 65535) {
                return RS_ERR_RANGE;
            }
            if (i >= 7) {
                e = rs_p1(dst, 0);
            } else if (id < 256) {
                e = rs_p2(dst, 0x100 + id);
            } else {
                e = rs_p2(dst, id);
            }
            if (e) {
                return e;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        if (a->colours[i] < 0 || a->colours[i] > 255) {
            return RS_ERR_RANGE;
        }
        e = rs_p1(dst, a->colours[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < 7; i++) {
        int anim = a->anims[i];
        if (anim < 0) {
            anim = 65535;
        }
        if (anim > 65535) {
            return RS_ERR_RANGE;
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

static rs_err appearance_read(rs_buf *src, rs_appearance *a) {
    int i;
    uint64_t name;
    rs_err e;
    memset(a, 0, sizeof(*a));
    e = need(src, 2);
    if (e) {
        return e;
    }
    a->gender = rs_g1(src);
    a->headicon = rs_g1(src);
    for (i = 0; i < 12; i++) {
        int hi, word;
        e = need(src, 1);
        if (e) {
            return e;
        }
        hi = rs_g1(src);
        if (hi == 0) {
            continue;
        }
        e = need(src, 1);
        if (e) {
            return e;
        }
        word = (hi << 8) | rs_g1(src);
        if (i == 0 && word == 65535) {
            e = need(src, 2);
            if (e) {
                return e;
            }
            a->npc_id = rs_g2(src);
            break;
        }
        if (i >= 7) {
            return RS_ERR_UNSUPPORTED;
        }
        a->styles[i] = word < 512 ? word - 256 : word;
    }
    e = need(src, 5 + 14 + 8 + 1 + 2);
    if (e) {
        return e;
    }
    for (i = 0; i < 5; i++) {
        a->colours[i] = rs_g1(src);
    }
    for (i = 0; i < 7; i++) {
        int anim = rs_g2(src);
        a->anims[i] = anim == 65535 ? -1 : anim;
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

static rs_err put_appearance_mask(rs_buf *dst, const rs_appearance *a) {
    uint8_t tmp[128];
    rs_buf body;
    rs_err e;
    int n;
    rs_buf_wrap(&body, tmp, sizeof tmp);
    e = appearance_write(&body, a);
    if (e) {
        return e;
    }
    n = (int)body.pos;
    if (n < 0 || n > 255) {
        return RS_ERR_RANGE;
    }
    e = rs_p1(dst, 1);
    if (e) {
        return e;
    }
    e = rs_p1(dst, n);
    if (e) {
        return e;
    }
    return rs_pdata(dst, tmp, (size_t)n);
}

static rs_err get_appearance_mask(rs_buf *src, rs_appearance *a, int *present) {
    int m, n;
    rs_err e;
    uint8_t tmp[256];
    rs_buf body;
    *present = 0;
    e = need(src, 1);
    if (e) {
        return e;
    }
    m = rs_g1(src);
    if (m & 128) {
        e = need(src, 1);
        if (e) {
            return e;
        }
        m += rs_g1(src) << 8;
    }
    /* Bit 1 is appearance. Bit 128 only extends the mask. Other blocks were not read. */
    if ((m & ~129) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if ((m & 1) == 0) {
        return RS_OK;
    }
    e = need(src, 1);
    if (e) {
        return e;
    }
    n = rs_g1(src);
    e = need(src, (size_t)n);
    if (e) {
        return e;
    }
    memcpy(tmp, src->data + src->pos, (size_t)n);
    src->pos += (size_t)n;
    rs_buf_wrap(&body, tmp, (size_t)n);
    e = appearance_read(&body, a);
    if (e) {
        return e;
    }
    *present = 1;
    return RS_OK;
}

/* Local player only. Client.opClient41, then readPlayers count 0, getPlayerNewVis 2047.
   Teleport bit order is plane, local x, local z, snap, mask. Appearance mask bit is 1. */
static rs_err player_encode(const rs_session *s, rs_buf *dst, const rs_move_step *step) {
    int walk, run, mask;
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
            e = rs_pbit(dst, 2, step->plane & 3);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 7, step->local_x & 127);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 7, step->local_z & 127);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, 1);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, mask);
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
            /* type 0 always queues a mask byte */
            e = rs_pbit(dst, 2, 0);
            mask = 1;
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
    if (!mask) {
        return RS_OK;
    }
    if (!step->appearance) {
        return rs_p1(dst, 0);
    }
    return put_appearance_mask(dst, &step->look);
}

static rs_err player_decode(const rs_session *s, rs_buf *src, rs_move_step *step) {
    int flag, type, upd, count, id, present;
    rs_err e;
    (void)s;
    if (!src || !step) {
        return RS_ERR_ARG;
    }
    memset(step, 0, sizeof(*step));
    step->dir1 = -1;
    step->dir2 = -1;
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
        } else if (type == 3) {
            int snap;
            step->teleport = 1;
            e = gbits(src, 2, &step->plane);
            if (e) {
                return e;
            }
            e = gbits(src, 7, &step->local_x);
            if (e) {
                return e;
            }
            e = gbits(src, 7, &step->local_z);
            if (e) {
                return e;
            }
            e = gbits(src, 1, &snap);
            if (e) {
                return e;
            }
            (void)snap;
            e = gbits(src, 1, &upd);
        } else {
            return RS_ERR_UNSUPPORTED;
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
        if (rs_buf_remaining(src) != 0) {
            return RS_ERR_FORMAT;
        }
        return RS_OK;
    }
    e = get_appearance_mask(src, &step->look, &present);
    if (e) {
        return e;
    }
    step->appearance = present;
    if (rs_buf_remaining(src) != 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

/* Payload after the opcode. -1 is one length byte, -3 was not written.
 * 207 and 86 are walk_encode with nsteps == 0. The client also writes a
 * length byte, and minimap callers add 14 bytes this encoder does not write.
 */
static const int8_t k_c2s_282[256] = {
      1,  -3,   6,  -3,  -3,  -3,  -3,  -3,  -3,   2,  -3,  -3,  -1,   8,  -3,  -3,
     -3,  -3,  -3,  -3,   4,  -3,  -3,  -3,   3,   4,  -3,  -3,  -3,  -3,  -3,  -3,
     -3,  -3,  -3,  -3,   8,  -3,  -3,  12,  -3,   4,   6,  -3,  -3,  -3,   6,  -3,
     -3,  -3,  -3,   0,   1,   4,  -3,  -3,  -3,  -3,  -3,  -3,  12,  -3,   6,  -3,
     -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,   2,   1,   6,  -3,  -3,  -3,  -3,  -3,
      1,  -3,   6,  -3,  -3,  -3,   5,  -3,  -3,   0,  -3,   8,  -3,   7,   1,  -3,
     -3,  -3,   2,  -3,   0,   8,   4,   6,  -3,  -3,   8,  -3,   6,   2,  -3,  -3,
     -3,  -3,  -3,  -3,  -3,   6,  -3,  -3,   0,  -3,  -3,   6,  -3,  13,  -3,   6,
     -3,  -3,  -3,  -3,  -3,  -3,  -3,   8,  12,  10,  -1,  -1,  -3,  -3,  -3,  -3,
     -3,  -3,  -3,   2,  -3,  -1,   8,  -3,  -3,  -3,   3,  -3,  -3,   6,  -3,  -3,
     -3,  -3,  -3,  -3,  -3,  -3,   2,  -3,  -3,   6,  -3,  -3,  -3,  -3,   2,  -3,
     -3,  -3,  -3,   6,  -3,   4,  -3,  -3,  -3,   6,  -3,   6,   1,   2,  -3,  -3,
     -3,  -3,  -3,  -3,   2,  -3,  -3,  -3,  -3,   2,  -3,  -3,  -3,  -3,  -3,   5,
     -3,   0,  -3,  -3,  -3,   8,   0,   6,   6,  -3,  -3,   4,  -3,  -3,  -1,   2,
     -1,  -3,  -3,  -3,  -3,  -3,   1,  -3,  -3,   2,  -3,   1,   2,  -3,  -3,   6,
      4,  -3,  -3,  -3,  -3,  -3,  -3,   6,  -3,  -3,   2,  -3,  -3,  -1,  -3,   8,
};

static const rs_move_layout k_layout = {
    282,
    k_c2s_282,
    rs_sizes_s2c_282,
    207, /* Client.opClient8 var11 == 0 */
    86,  /* var11 == 1, minimap */
    231, /* Client.read rebuild */
    167, /* Client.read -> readPlayerInfo */
    -1,
    0,
    0,
    walk_encode,
    walk_decode,
    region_encode,
    region_decode,
    player_encode,
    player_decode};

void rs_move_282_init(void) {
    rs_move_register(&k_layout);
}

static void rs_move_282_ctor(void) __attribute__((constructor));
static void rs_move_282_ctor(void) {
    rs_move_282_init();
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

static int same_look(const rs_appearance *a, const rs_appearance *b) {
    return a->gender == b->gender && a->headicon == b->headicon && a->combat_level == b->combat_level &&
           a->skill_level == b->skill_level && a->npc_id == b->npc_id &&
           memcmp(a->colours, b->colours, sizeof a->colours) == 0 &&
           memcmp(a->styles, b->styles, sizeof a->styles) == 0 &&
           memcmp(a->anims, b->anims, sizeof a->anims) == 0 && strcmp(a->name, b->name) == 0;
}

static void fill_look(rs_appearance *a, int npc) {
    int i;
    memset(a, 0, sizeof(*a));
    a->gender = 0;
    a->headicon = 1;
    a->colours[0] = 2;
    a->colours[1] = 3;
    a->colours[2] = 4;
    a->colours[3] = 5;
    a->colours[4] = 6;
    for (i = 0; i < 7; i++) {
        a->styles[i] = 10 + i;
        a->anims[i] = 800 + i;
    }
    a->anims[0] = -1;
    a->npc_id = npc;
    memcpy(a->name, "bob", 4);
    a->combat_level = 12;
    a->skill_level = 42;
}

static void round_step(const rs_move_step *in, const char *label) {
    rs_buf dst, src;
    rs_move_step out;
    expect(rs_buf_alloc(&dst, 256) == RS_OK, label);
    expect(player_encode(NULL, &dst, in) == RS_OK, label);
    rs_buf_wrap(&src, dst.data, dst.pos);
    expect(player_decode(NULL, &src, &out) == RS_OK, label);
    expect(out.teleport == in->teleport && out.plane == (in->teleport ? in->plane : 0), label);
    expect(out.local_x == (in->teleport ? in->local_x : 0), label);
    expect(out.local_z == (in->teleport ? in->local_z : 0), label);
    expect(out.dir1 == (in->teleport ? -1 : in->dir1), label);
    expect(out.dir2 == (in->teleport ? -1 : in->dir2), label);
    expect(out.appearance == in->appearance, label);
    if (in->appearance) {
        rs_appearance want = in->look;
        if (want.npc_id > 0) {
            memset(want.styles, 0, sizeof want.styles);
        }
        expect(same_look(&out.look, &want), label);
    }
    rs_buf_free(&dst);
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
    expect(out.running && out.dest_x == 3223 && out.dest_z == 3222 && out.nsteps == 1, "walk fields");
    expect(out.dx[0] == 1 && out.dz[0] == -1, "walk step");
    rs_buf_free(&dst);
}

static void round_region(void) {
    rs_buf dst, src;
    rs_region in, out;
    memset(&in, 0, sizeof in);
    in.zone_x = 400;
    in.zone_z = 401;
    in.local_x = 20;
    expect(rs_buf_alloc(&dst, 16) == RS_OK, "region alloc");
    expect(region_encode(NULL, &dst, &in) == RS_OK, "region encode");
    expect(dst.pos == 4 && dst.data[0] == 0x01 && dst.data[1] == 0x90 && dst.data[2] == 0x01 && dst.data[3] == 0x91,
           "region bytes");
    rs_buf_wrap(&src, dst.data, dst.pos);
    expect(region_decode(NULL, &src, &out) == RS_OK, "region decode");
    expect(out.zone_x == 400 && out.zone_z == 401 && out.local_x == 0 && out.local_z == 0, "region fields");
    rs_buf_free(&dst);
}

int main(void) {
    const rs_move_layout *m;
    const rs_build_info *info;
    rs_move_step step;
    rs_move_282_init();
    m = rs_move_for(282);
    info = rs_build_lookup(282);
    expect(m != NULL && m->sizes_c2s == k_c2s_282 && m->sizes_s2c == rs_sizes_s2c_282, "sizes");
    {
        rs_buf dst;
        rs_walk z;
        memset(&z, 0, sizeof z);
        z.dest_x = 3222;
        z.dest_z = 3222;
        expect(rs_buf_alloc(&dst, 32) == RS_OK, "c2s walk");
        expect(walk_encode(NULL, &dst, &z) == RS_OK && m->sizes_c2s[m->opcode_walk] == (int)dst.pos, "c2s walk size");
        expect(m->sizes_c2s[1] == -3, "c2s unknown");
        rs_buf_free(&dst);
    }
    expect(m && m->walk_encode && m->region_encode && m->player_encode, "fns");
    expect(rs_pkt_opcode(info, RS_C2S_WALK) == 207, "walk opcode");
    expect(rs_pkt_opcode(info, RS_C2S_WALK_MINIMAP) == 86, "minimap opcode");
    expect(rs_pkt_opcode(info, RS_S2C_REGION) == 231, "region opcode");
    expect(rs_pkt_opcode(info, RS_S2C_PLAYER_UPDATE) == 167, "player opcode");
    round_walk();
    round_region();
    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.plane = 1;
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = -1;
    step.dir2 = -1;
    step.appearance = 1;
    fill_look(&step.look, 0);
    round_step(&step, "teleport appearance");
    step.appearance = 1;
    fill_look(&step.look, 50);
    round_step(&step, "teleport npc");
    memset(&step, 0, sizeof step);
    step.dir1 = 4;
    step.dir2 = -1;
    round_step(&step, "dir1 walk");
    step.dir1 = 4;
    step.dir2 = 6;
    round_step(&step, "dir2 run");
    expect(player_encode(NULL, NULL, &step) == RS_ERR_ARG, "player arg");
    step.dir1 = 9;
    step.dir2 = -1;
    {
        rs_buf dst;
        expect(rs_buf_alloc(&dst, 32) == RS_OK, "bad alloc");
        expect(player_encode(NULL, &dst, &step) == RS_ERR_UNSUPPORTED, "bad dir");
        rs_buf_free(&dst);
    }
    if (g_fail) {
        fprintf(stderr, "282 move test failed\n");
        return 1;
    }
    printf("282 move ok\n");
    return 0;
}
#endif
