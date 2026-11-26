#include "rslib/both/move.h"

#include <string.h>

/*
 * 614 KeyHeldBinding.opKeyHeldBinding4. Game click FloorUnderlayType(26, 5)
 * at PathingEntity_Class147.valuePathingEntity_Class147110. Minimap
 * FloorUnderlayType(7, 18) at ClanChannelUser.valueClanChannelUser20.
 * The constructor keeps only the opcode. Body is Unused6.opUnused6161
 * (p1_add128 running), opUnused6170 (p2_le world z), opUnused6167
 * (p2_add128 world x). No step bytes. Minimap appends 13 client bytes.
 * MdaWrite.opMdaWrite1 indexes server packets: player
 * KbaFinalize.valueKbaFinalize6 is 64, rebuild
 * StaticPointLight.valueStaticPointLight15 is 10. Region bytes stay in
 * rs_region_encode_zones. LoginProt's int[256] is a palette histogram, not
 * packet lengths, so these pointers stay null. Player block is
 * KeaInit.opKeaInit2: RendererToolkit.callf97538dc,
 * NativeLibraryLoader.opNativeLibraryLoader3, C0e0d8b98.opkind11082, then
 * Js5ArchiveResourceLoader.opJs5ArchiveResourceLoader3 /
 * TextureOp14.opTextureOp1427. Appearance is mask bit 64
 * (C0dc71316.opkind105817). Length is call08b1455d (g1_neg). opUnused6148
 * reverses the bytes and subtracts 128.
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
    e = rs_p1_add128(dst, w->running ? 1 : 0);
    if (e) {
        return e;
    }
    e = rs_p2_le(dst, w->dest_z);
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
    w->running = rs_g1_add128(src);
    w->dest_z = rs_g2_le(src);
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
    len = rs_g1_neg(src);
    if (len < 0 || len > (int)sizeof raw) {
        return RS_ERR_FORMAT;
    }
    for (i = len - 1; i >= 0; i--) {
        raw[i] = (uint8_t)rs_g1_add128(src);
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
    e = rs_p1(dst, 64);
    if (e) {
        return e;
    }
    e = rs_p1_neg(dst, (int)logical.pos);
    if (e) {
        return e;
    }
    for (i = (int)logical.pos - 1; i >= 0; i--) {
        e = rs_p1_add128(dst, raw[i]);
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
    if (mask & 32) {
        mask |= rs_g1(src) << 8;
    }
    if (mask & 0x8000) {
        mask |= rs_g1(src) << 16;
    }
    if (mask & ~64) {
        return RS_ERR_UNSUPPORTED;
    }
    if (mask & 64) {
        step->appearance = 1;
        e = take_look(src, &step->look);
        if (e) {
            return e;
        }
    }
    return rs_buf_remaining(src) == 0 ? RS_OK : RS_ERR_FORMAT;
}

static const rs_move_layout k_614 = {
    614,
    NULL,
    NULL,
    26,
    7,
    10,
    64,
    -1,
    0,
    0,
    walk_encode,
    walk_decode,
    NULL,
    NULL,
    player_encode,
    player_decode
};

__attribute__((constructor)) static void rs_move_614_init(void) {
    rs_move_register(&k_614);
}

#ifdef RS_MOVE_TEST
#include <stdio.h>

static int fail(const char *msg) {
    fprintf(stderr, "614 %s\n", msg);
    return 1;
}

int main(void) {
    rs_buf buf;
    rs_walk in, out;
    rs_move_step step, got;
    const rs_move_layout *m;
    uint8_t raw[512];
    /* run, le z 3222, add128 x 3222. */
    static const uint8_t expect[] = {0x80, 0x96, 0x0c, 0x0c, 0x16};

    rs_move_614_init();
    m = rs_move_for(614);
    if (!m || m->opcode_walk != 26 || m->opcode_walk_minimap != 7 || m->opcode_region != 10 ||
        m->opcode_player != 64 || m->opcode_player_index != -1 || m->walk_encode == NULL ||
        m->region_encode != NULL || m->player_encode == NULL || m->sizes_c2s != NULL ||
        m->sizes_s2c != NULL) {
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
    return 0;
}
#endif
