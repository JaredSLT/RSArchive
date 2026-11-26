#include "rslib/both/move.h"

#include <string.h>

/*
 * 530. SecondaryNode opcode 225 (~JavaMouseWheel.valueJavaMouseWheel121 == -226)
 * calls HashTable.opHashTable27. Local BrokenInputStream.opBrokenInputStream4,
 * others QuickChatCatType.opQuickChatCatType19 (8-bit count, "gppov1"),
 * add C20d1fcdf.opkind6273 (11-bit index, stop at 2047).
 * Masks C630c84e5.opkind6044, extension bit 0x10. Appearance bit 4 is
 * Js5Request.opJs5Request1; length is opUnused653 and the bytes are plain.
 * Look block LocType.opLocType21 writes one anim. Walk body
 * ComponentPointer.opComponentPointer13 (opcodes 215 / 39 / 77).
 * Region rebuild stays rs_region_encode_530 (opcode 162).
 * Type 2 with the leading bit clear is one direction at speed 0. That
 * decodes as dir1 set and dir2 -1, and encodes again as type 1.
 */

static const int k_body_anim_317[7] = {0x328, 0x337, 0x333, 0x334, 0x335, 0x336, 0x338};

enum {
    k_mask_appear = 0x04,
    k_mask_ext = 0x10,
    k_anims = 1
};

static rs_err need(rs_buf *b, size_t n) {
    if (!b || rs_buf_remaining(b) < n) {
        return RS_ERR_NEED_MORE;
    }
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

static int anim_wire(int anim, int index) {
    if (anim == 0) {
        return k_body_anim_317[index];
    }
    if (anim < 0) {
        return 65535;
    }
    return anim;
}

/* LocType.opLocType21. One body anim. Npc id is followed by one extra byte (valueLocType115), written 0. */
static rs_err look_write(rs_buf *dst, const rs_appearance *a) {
    int i;
    rs_err e;
    int flags;
    if (a->gender != 0 && a->gender != 1) {
        return RS_ERR_RANGE;
    }
    if (a->headicon < -128 || a->headicon > 127) {
        return RS_ERR_RANGE;
    }
    if (a->combat_level < 0 || a->combat_level > 255 || a->skill_level < 0 || a->skill_level > 65535) {
        return RS_ERR_RANGE;
    }
    if (a->npc_id < 0 || a->npc_id > 65535) {
        return RS_ERR_RANGE;
    }
    flags = (a->gender & 1) | (a->skill_level != 0 ? 4 : 0);
    e = rs_p1(dst, flags);
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
        e = rs_p1(dst, 0);
        if (e) {
            return e;
        }
    } else {
        for (i = 0; i < 12; i++) {
            int id = (i < 7) ? a->styles[i] : 0;
            if (i < 7 && (id < 0 || id > 32511)) {
                return RS_ERR_RANGE;
            }
            e = (i < 7) ? rs_p2(dst, 0x100 + id) : rs_p1(dst, 0);
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
    for (i = 0; i < k_anims; i++) {
        int anim = a->anims[i];
        if (anim > 65535) {
            return RS_ERR_RANGE;
        }
        e = rs_p2(dst, anim_wire(anim, i));
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
    if (a->skill_level != 0) {
        e = rs_p2(dst, a->skill_level);
        if (e) {
            return e;
        }
    } else {
        e = rs_p1(dst, 0);
        if (e) {
            return e;
        }
        e = rs_p1(dst, 255);
        if (e) {
            return e;
        }
    }
    return rs_p1(dst, 0);
}

static rs_err look_read(rs_buf *src, rs_appearance *a) {
    int i;
    int flags;
    int scale;
    uint64_t name;
    rs_err e;
    memset(a, 0, sizeof(*a));
    e = need(src, 1);
    if (e) {
        return e;
    }
    flags = rs_g1(src);
    a->gender = flags & 1;
    e = need(src, 2);
    if (e) {
        return e;
    }
    a->headicon = rs_g1b(src);
    (void)rs_g1b(src);
    for (i = 0; i < 12; i++) {
        int hi;
        int word;
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
            e = need(src, 3);
            if (e) {
                return e;
            }
            a->npc_id = rs_g2(src);
            (void)rs_g1(src);
            break;
        }
        if (i >= 7) {
            continue;
        }
        if (word >= 32768) {
            return RS_ERR_UNSUPPORTED;
        }
        a->styles[i] = word - 256;
    }
    e = need(src, 5);
    if (e) {
        return e;
    }
    for (i = 0; i < 5; i++) {
        a->colours[i] = rs_g1(src);
    }
    e = need(src, (size_t)k_anims * 2);
    if (e) {
        return e;
    }
    for (i = 0; i < k_anims; i++) {
        int anim = rs_g2(src);
        a->anims[i] = anim == 65535 ? -1 : anim;
    }
    e = need(src, 8 + 1);
    if (e) {
        return e;
    }
    name = rs_g8(src);
    if (name == 0) {
        a->name[0] = 0;
    } else {
        rs_base37_decode(name, a->name);
    }
    a->combat_level = rs_g1(src);
    if ((flags & 4) != 0) {
        e = need(src, 2);
        if (e) {
            return e;
        }
        a->skill_level = rs_g2(src);
    } else {
        e = need(src, 2);
        if (e) {
            return e;
        }
        (void)rs_g1(src);
        (void)rs_g1(src);
    }
    e = need(src, 1);
    if (e) {
        return e;
    }
    scale = rs_g1(src);
    if (scale != 0) {
        e = need(src, 8);
        if (e) {
            return e;
        }
        (void)rs_g2(src);
        (void)rs_g2(src);
        (void)rs_g2(src);
        (void)rs_g2(src);
    }
    if (rs_buf_remaining(src) != 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err look_pack(const rs_appearance *a, uint8_t *raw, int *n) {
    rs_buf body;
    rs_err e;
    rs_buf_wrap(&body, raw, 256);
    e = look_write(&body, a);
    if (e) {
        return e;
    }
    if (body.pos > 255) {
        return RS_ERR_RANGE;
    }
    *n = (int)body.pos;
    return RS_OK;
}

static rs_err mask_write(rs_buf *dst, const uint8_t *raw, int n) {
    rs_err e = rs_p1(dst, k_mask_appear);
    if (e) {
        return e;
    }
    e = rs_p1_add128(dst, n);
    if (e) {
        return e;
    }
    return rs_pdata(dst, raw, (size_t)n);
}

static rs_err mask_read(rs_buf *src, rs_move_step *step) {
    int m;
    int n;
    int i;
    uint8_t tmp[256];
    rs_buf body;
    rs_err e;
    e = need(src, 1);
    if (e) {
        return e;
    }
    m = rs_g1(src);
    if ((m & k_mask_ext) != 0) {
        e = need(src, 1);
        if (e) {
            return e;
        }
        m |= rs_g1(src) << 8;
    }
    if ((m & ~(k_mask_appear | k_mask_ext)) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if ((m & k_mask_appear) == 0) {
        return RS_OK;
    }
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
        tmp[i] = (uint8_t)rs_g1(src);
    }
    rs_buf_wrap(&body, tmp, (size_t)n);
    e = look_read(&body, &step->look);
    if (e) {
        return e;
    }
    step->appearance = 1;
    return RS_OK;
}

/* BrokenInputStream.opBrokenInputStream4. Teleport order is z, jump, plane, mask, x. Jump is written 1. */
static rs_err player_encode(const rs_session *s, rs_buf *dst, const rs_move_step *step) {
    int walk;
    int run;
    int mask;
    int queued;
    uint8_t lookb[256];
    int lookn = 0;
    rs_err e;
    (void)s;
    if (!dst || !step) {
        return RS_ERR_ARG;
    }
    walk = step->dir1 >= 0 && step->dir1 <= 7;
    run = step->dir2 >= 0 && step->dir2 <= 7;
    if (step->teleport) {
        if (step->plane < 0 || step->plane > 3 || step->local_x < 0 || step->local_x > 127 || step->local_z < 0 ||
            step->local_z > 127) {
            return RS_ERR_RANGE;
        }
    } else if (run && !walk) {
        return RS_ERR_UNSUPPORTED;
    } else if (!walk && (step->dir1 >= 0 || step->dir2 >= 0)) {
        return RS_ERR_UNSUPPORTED;
    }
    mask = step->appearance ? 1 : 0;
    if (mask) {
        e = look_pack(&step->look, lookb, &lookn);
        if (e) {
            return e;
        }
    }
    queued = 0;
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
            e = rs_pbit(dst, 7, step->local_z & 127);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, 1);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 2, step->plane & 3);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, mask);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 7, step->local_x & 127);
            queued = mask;
        } else if (run) {
            e = rs_pbit(dst, 2, 2);
            if (e) {
                return e;
            }
            e = rs_pbit(dst, 1, 1);
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
            queued = mask;
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
            queued = mask;
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
    return mask_write(dst, lookb, lookn);
}

static rs_err player_decode(const rs_session *s, rs_buf *src, rs_move_step *step) {
    int flag;
    int type;
    int upd;
    int count;
    int id;
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
            int two;
            e = gbits(src, 1, &two);
            if (e) {
                return e;
            }
            if (two) {
                e = gbits(src, 3, &step->dir1);
                if (e) {
                    return e;
                }
                e = gbits(src, 3, &step->dir2);
            } else {
                e = gbits(src, 3, &step->dir1);
            }
            if (e) {
                return e;
            }
            e = gbits(src, 1, &upd);
        } else {
            int jump;
            step->teleport = 1;
            e = gbits(src, 7, &step->local_z);
            if (e) {
                return e;
            }
            e = gbits(src, 1, &jump);
            if (e) {
                return e;
            }
            (void)jump;
            e = gbits(src, 2, &step->plane);
            if (e) {
                return e;
            }
            e = gbits(src, 1, &upd);
            if (e) {
                return e;
            }
            e = gbits(src, 7, &step->local_x);
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
    if (upd) {
        e = mask_read(src, step);
        if (e) {
            return e;
        }
    }
    if (rs_buf_remaining(src) != 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

/* ComponentPointer.opComponentPointer13: p1_add128 run, plain p2 x, p2_add128 z, p1_add128 dx, p1_sub128 dz. */
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
    e = rs_p1_add128(dst, w->running ? 1 : 0);
    if (e) {
        return e;
    }
    e = rs_p2(dst, w->dest_x);
    if (e) {
        return e;
    }
    e = rs_p2_add128(dst, w->dest_z);
    if (e) {
        return e;
    }
    for (i = 0; i < w->nsteps; i++) {
        e = rs_p1_add128(dst, w->dx[i]);
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

static rs_err walk_decode(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 5) {
        return RS_ERR_NEED_MORE;
    }
    memset(w, 0, sizeof(*w));
    w->running = rs_g1_add128(src) != 0;
    w->dest_x = rs_g2(src);
    w->dest_z = rs_g2_add128(src);
    while (rs_buf_remaining(src) >= 2) {
        if (w->nsteps >= RS_WALK_STEPS) {
            return RS_ERR_RANGE;
        }
        w->dx[w->nsteps] = (int8_t)rs_g1_add128(src);
        w->dz[w->nsteps] = (int8_t)rs_g1_sub128(src);
        w->nsteps++;
    }
    if (rs_buf_remaining(src) != 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static const rs_move_layout k_layout = {
    530,
    NULL,
    NULL,
    215, /* ComponentPointer.opComponentPointer13 var1 == 0 */
    39,  /* var1 == 1, minimap */
    162, /* SecondaryNode -> C633b3b0d.opkind5884(false) */
    225, /* ~valueJavaMouseWheel121 == -226 -> HashTable.opHashTable27 */
    -1,
    0,
    0,
    walk_encode,
    walk_decode,
    NULL,
    NULL,
    player_encode,
    player_decode};

__attribute__((constructor)) static void rs_move_530_init(void) {
    rs_move_register(&k_layout);
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

static void dump(const char *msg, const uint8_t *p, size_t n) {
    size_t i;
    fprintf(stderr, "%s", msg);
    for (i = 0; i < n; i++) {
        fprintf(stderr, " %02x", p[i]);
    }
    fprintf(stderr, "\n");
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

static int same_look(const rs_appearance *a, const rs_appearance *b) {
    return a->gender == b->gender && a->headicon == b->headicon && a->combat_level == b->combat_level &&
           a->skill_level == b->skill_level && a->npc_id == b->npc_id && memcmp(a->colours, b->colours, sizeof a->colours) == 0 &&
           memcmp(a->styles, b->styles, sizeof a->styles) == 0 && memcmp(a->anims, b->anims, sizeof a->anims) == 0 &&
           strcmp(a->name, b->name) == 0;
}

static void fill_look(rs_appearance *a, int npc, int skill) {
    int i;
    memset(a, 0, sizeof(*a));
    a->gender = 1;
    a->headicon = 3;
    for (i = 0; i < 5; i++) {
        a->colours[i] = 2 + i;
    }
    for (i = 0; i < 7; i++) {
        a->styles[i] = 10 + i;
        a->anims[i] = 800 + i;
    }
    a->anims[0] = -1;
    a->npc_id = npc;
    memcpy(a->name, "bob", 4);
    a->combat_level = 12;
    a->skill_level = skill;
}

static void expect_look(const rs_appearance *got, const rs_appearance *in) {
    rs_appearance want = *in;
    int i;
    if (want.npc_id > 0) {
        memset(want.styles, 0, sizeof want.styles);
    }
    for (i = 1; i < 7; i++) {
        want.anims[i] = 0;
    }
    expect(same_look(got, &want), "look");
}

static void round_step(const rs_move_step *in, const char *label) {
    uint8_t raw[256];
    uint8_t again[256];
    rs_buf dst;
    rs_buf src;
    rs_move_step out;
    rs_move_step back;
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&dst, raw, sizeof raw);
    expect(player_encode(NULL, &dst, in) == RS_OK, label);
    rs_buf_wrap(&src, raw, dst.pos);
    expect(player_decode(NULL, &src, &out) == RS_OK, label);
    expect(out.teleport == in->teleport, label);
    expect(out.plane == (in->teleport ? in->plane : 0), label);
    expect(out.local_x == (in->teleport ? in->local_x : 0), label);
    expect(out.local_z == (in->teleport ? in->local_z : 0), label);
    expect(out.dir1 == (in->teleport ? -1 : in->dir1), label);
    expect(out.dir2 == (in->teleport ? -1 : in->dir2), label);
    expect(out.appearance == in->appearance, label);
    if (in->appearance) {
        expect_look(&out.look, &in->look);
    }
    memset(again, 0, sizeof again);
    rs_buf_wrap(&dst, again, sizeof again);
    back = out;
    if (!in->teleport) {
        back.dir1 = in->dir1;
        back.dir2 = in->dir2;
    }
    expect(player_encode(NULL, &dst, &back) == RS_OK, label);
    if (dst.pos != src.cap || memcmp(raw, again, dst.pos) != 0) {
        dump("first", raw, src.cap);
        dump("second", again, dst.pos);
        expect(0, label);
    }
}

static void round_walk(const rs_walk *in, const uint8_t *expect_bytes, size_t n, const char *label) {
    uint8_t raw[64];
    rs_buf dst;
    rs_buf src;
    rs_walk out;
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&dst, raw, sizeof raw);
    expect(walk_encode(NULL, &dst, in) == RS_OK, label);
    if (dst.pos != n || memcmp(raw, expect_bytes, n) != 0) {
        dump(label, raw, dst.pos);
        expect(0, label);
    }
    rs_buf_wrap(&src, raw, dst.pos);
    expect(walk_decode(NULL, &src, &out) == RS_OK && same_walk(in, &out), label);
}

int main(void) {
    const rs_build_info *info;
    const rs_move_layout *m;
    rs_move_step step;
    rs_move_step out;
    rs_walk walk;
    uint8_t raw[32];
    uint8_t again[32];
    rs_buf dst;
    rs_buf src;
    static const uint8_t idle[] = {0x00, 0x7f, 0xf0};
    static const uint8_t teleport[] = {0xe2, 0xa9, 0xb0, 0x07, 0xff};
    static const uint8_t one_step[] = {0x81, 0x0c, 0x97, 0x0c, 0x16, 0x7f, 0x7f};
    static const uint8_t no_step[] = {0x80, 0x0c, 0x97, 0x0c, 0x16};

    info = rs_build_lookup(530);
    expect(info != NULL, "lookup");
    m = rs_move_for(530);
    expect(m && m->opcode_walk == 215 && m->opcode_walk_minimap == 39 && m->opcode_region == 162 && m->opcode_player == 225 &&
               m->opcode_player_index == -1 && m->sizes_c2s == NULL && m->region_encode == NULL && m->walk_encode && m->player_encode,
           "layout");
    expect(info && rs_pkt_opcode(info, RS_C2S_WALK) == 215, "walk opcode");
    expect(info && rs_move_playable(info), "playable");

    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&dst, raw, sizeof raw);
    memset(&step, 0, sizeof step);
    step.dir1 = -1;
    step.dir2 = -1;
    expect(player_encode(NULL, &dst, &step) == RS_OK && dst.pos == sizeof idle && memcmp(raw, idle, sizeof idle) == 0, "idle bytes");

    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&dst, raw, sizeof raw);
    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.plane = 1;
    step.local_x = 54;
    step.local_z = 10;
    step.dir1 = -1;
    step.dir2 = -1;
    expect(player_encode(NULL, &dst, &step) == RS_OK && dst.pos == sizeof teleport && memcmp(raw, teleport, sizeof teleport) == 0,
           "teleport bytes");

    memset(&step, 0, sizeof step);
    step.teleport = 1;
    step.plane = 0;
    step.local_x = 54;
    step.local_z = 54;
    step.dir1 = -1;
    step.dir2 = -1;
    step.appearance = 1;
    fill_look(&step.look, 0, 42);
    round_step(&step, "teleport look");
    fill_look(&step.look, 90, 0);
    round_step(&step, "teleport npc");

    memset(&step, 0, sizeof step);
    step.dir1 = 2;
    step.dir2 = 6;
    round_step(&step, "run dir");

    /* Type 2, leading bit 0, direction 4, no mask: crawl. Re-encode is type 1. */
    memset(raw, 0, sizeof raw);
    rs_buf_wrap(&dst, raw, sizeof raw);
    rs_access_bits(&dst);
    expect(rs_pbit(&dst, 1, 1) == RS_OK && rs_pbit(&dst, 2, 2) == RS_OK && rs_pbit(&dst, 1, 0) == RS_OK && rs_pbit(&dst, 3, 4) == RS_OK &&
               rs_pbit(&dst, 1, 0) == RS_OK && rs_pbit(&dst, 8, 0) == RS_OK && rs_pbit(&dst, 11, 2047) == RS_OK,
           "crawl bits");
    rs_access_bytes(&dst);
    rs_buf_wrap(&src, raw, dst.pos);
    expect(player_decode(NULL, &src, &out) == RS_OK && out.dir1 == 4 && out.dir2 == -1 && out.teleport == 0 && out.appearance == 0,
           "crawl decode");
    memset(again, 0, sizeof again);
    rs_buf_wrap(&dst, again, sizeof again);
    expect(player_encode(NULL, &dst, &out) == RS_OK, "crawl reencode");
    expect(dst.pos != src.cap || memcmp(raw, again, dst.pos) != 0, "crawl not identical");

    memset(&walk, 0, sizeof walk);
    walk.dest_x = 3223;
    walk.dest_z = 3222;
    round_walk(&walk, no_step, sizeof no_step, "walk idle");
    walk.running = 1;
    walk.nsteps = 1;
    walk.dx[0] = -1;
    walk.dz[0] = 1;
    round_walk(&walk, one_step, sizeof one_step, "walk step");
    return g_fail ? 1 : 0;
}
#endif
