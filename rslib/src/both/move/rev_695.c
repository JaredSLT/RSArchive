#include "rslib/both/move.h"

#include <string.h>

/*
 * 695 c2s protos are Bwz.opkind25975 / class548.method4909(opcode, new Object[1],
 * size, dummy). Opcodes 0..96 are unique. 97..255 are absent, so they are -3.
 * There is no second prot constructor and no int[256] size array, so sizes_s2c
 * stays NULL.
 *
 * Game click Zqw.valuekind23663 / class338.field3535 is opcode 1, size 5.
 * Minimap Cfa8219d8.valuekind27061 / class646.field6564 is opcode 56, size 18.
 * class472.method4446 selects them with click-type local 8 (0 game, 1 minimap).
 * The game-click body is class486.method4531, class516.method4723, then
 * class493.method4581: p2_le_add128(base int[0] + menu coord), p2(base int[2] +
 * menu coord), p1_sub128(run 0/1). class8.method44's two g2s feed
 * class1031.method8700 / class342.method3248. class733.method6345 names the
 * maps "m"+x+"_"+z: the first g2 is z (first short) and the second g2 is x
 * (second short). The stored base is that zone times 8, so base + the menu
 * addend is the world tile. nsteps must be 0. The minimap path's extra bytes
 * are not written.
 *
 * class8.method44 is g2, g1_sub128, g2, g1_add128, then a remaining/16 tail
 * that was not read as keys. It does not match rs_region_encode_zones, and the
 * tail is incomplete, so region_encode stays NULL. No player block with an
 * 11-bit 2045 skip was found (2047 is only a mask in class64 / class399).
 * opcode_player and opcode_player_index stay -1. Spawn stays 0,0.
 */

static const int8_t k_c2s_695[256] = {
    12, 5, 3, -1, 1, 4, 8, 8, 3, -1, -1, -1, 3, 3, 0, 4,
    11, -1, 7, 9, 3, -1, 7, 6, 7, 6, 3, 3, 15, 16, -1, -1,
    -2, 8, 3, 7, 3, 9, 3, -2, 9, -1, -1, -1, 0, 2, 8, 4,
    16, 4, 3, 11, -1, 1, 6, 3, 18, -1, 7, 3, -1, 4, 0, 8,
    8, -1, -1, -1, 2, 3, -1, 3, 4, 7, 0, 9, 17, 8,
    8, 4, 4, 9, 8, 3, -1, 3, 2, -1, -1, 2, -1, 9,
    8, 4, 8, -1, -1, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3
};

static rs_err walk_encode_695(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    rs_err e;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    if (w->nsteps != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    e = rs_p2_le_add128(dst, w->dest_z);
    if (e) {
        return e;
    }
    e = rs_p2(dst, w->dest_x);
    if (e) {
        return e;
    }
    return rs_p1_sub128(dst, w->running ? 1 : 0);
}

static rs_err walk_decode_695(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) != 5) {
        return RS_ERR_FORMAT;
    }
    memset(w, 0, sizeof(*w));
    w->dest_z = rs_g2_le_add128(src);
    w->dest_x = rs_g2(src);
    w->running = rs_g1_sub128(src) != 0;
    return RS_OK;
}

static const rs_move_layout k_695 = {
    695,
    k_c2s_695,
    NULL,
    1,
    56,
    -1,
    -1,
    -1,
    0,
    0,
    walk_encode_695,
    walk_decode_695,
    NULL,
    NULL,
    NULL,
    NULL
};

__attribute__((constructor)) static void rs_move_695_init(void) {
    rs_move_register(&k_695);
}

#ifdef RS_MOVE_TEST
#include <stdio.h>

static int fail(const char *msg) {
    fprintf(stderr, "695 %s\n", msg);
    return 1;
}

int main(void) {
    rs_buf buf;
    rs_walk in, out;
    const rs_move_layout *m;
    uint8_t raw[32];
    int i;
    /* z 3222 p2_le_add128, x 3222 p2, p1_sub128(1) = 127. */
    static const uint8_t expect[] = {0x16, 0x0c, 0x0c, 0x96, 0x7f};

    m = rs_move_for(695);
    if (!m || m->sizes_c2s != k_c2s_695 || m->sizes_s2c || m->opcode_walk != 1 ||
        m->opcode_walk_minimap != 56 || m->opcode_region != -1 || m->opcode_player != -1 ||
        m->opcode_player_index != -1 || m->walk_encode == NULL || m->region_encode ||
        m->player_encode) {
        return fail("layout");
    }
    if (m->sizes_c2s[1] != 5 || m->sizes_c2s[56] != 18 || m->sizes_c2s[14] != 0 ||
        m->sizes_c2s[3] != -1 || m->sizes_c2s[32] != -2 || m->sizes_c2s[97] != -3) {
        return fail("sizes");
    }
    for (i = 0; i < 256; i++) {
        if (i <= 96) {
            continue;
        }
        if (m->sizes_c2s[i] != -3) {
            return fail("unknown");
        }
    }
    memset(&in, 0, sizeof in);
    in.dest_x = 3222;
    in.dest_z = 3222;
    in.running = 1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (walk_encode_695(NULL, &buf, &in) != RS_OK) {
        return fail("walk encode");
    }
    if (buf.pos != sizeof expect || memcmp(raw, expect, sizeof expect) != 0) {
        return fail("walk bytes");
    }
    rs_buf_wrap(&buf, raw, sizeof expect);
    if (walk_decode_695(NULL, &buf, &out) != RS_OK || out.dest_x != 3222 || out.dest_z != 3222 ||
        out.running != 1 || out.nsteps != 0) {
        return fail("walk decode");
    }
    in.nsteps = 1;
    rs_buf_wrap(&buf, raw, sizeof raw);
    if (walk_encode_695(NULL, &buf, &in) != RS_ERR_UNSUPPORTED) {
        return fail("steps");
    }
    return 0;
}
#endif
