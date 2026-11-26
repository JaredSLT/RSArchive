#include "rslib/both/sync.h"

#include <stdio.h>
#include <string.h>

static int g_fail;

static void fail(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    g_fail = 1;
}

static void expect_mem(const char *what, const rs_buf *b, const uint8_t *want, size_t n) {
    size_t i;
    if (b->pos == n && memcmp(b->data, want, n) == 0) {
        return;
    }
    fprintf(stderr, "%s:", what);
    for (i = 0; i < b->pos && i < 24; i++) {
        fprintf(stderr, " %02X", b->data[i]);
    }
    fprintf(stderr, "\n");
    g_fail = 1;
}

static void fill_look(rs_sync_player *p, int rev);

static void blank_player(rs_sync_player *p) {
    memset(p, 0, sizeof(*p));
    p->dir1 = -1;
    p->dir2 = -1;
}

static int encode_of(rs_sync_view *v, rs_buf *b, int npc) {
    rs_err e;
    rs_buf_reset(b);
    e = npc ? rs_sync_npc_encode(v, b) : rs_sync_player_encode(v, b);
    if (e != RS_OK) {
        fprintf(stderr, "encode %s rev failed %d\n", npc ? "npc" : "player", (int)e);
        g_fail = 1;
        return 1;
    }
    return 0;
}

static int decode_of(rs_sync_view *v, const rs_buf *src, int npc) {
    rs_buf in;
    rs_err e;
    rs_buf_wrap(&in, src->data, src->pos);
    e = npc ? rs_sync_npc_decode(v, &in) : rs_sync_player_decode(v, &in);
    if (e != RS_OK) {
        fprintf(stderr, "decode %s failed %d\n", npc ? "npc" : "player", (int)e);
        g_fail = 1;
        return 1;
    }
    return 0;
}

static void test_refuse(int rev) {
    rs_sync_view *v;
    rs_buf b;
    uint8_t byte;
    char msg[64];
    v = rs_sync_view_create(rev);
    if (!v) {
        fprintf(stderr, "create %d failed\n", rev);
        g_fail = 1;
        return;
    }
    byte = 0;
    rs_buf_wrap(&b, &byte, 1);
    if (rs_sync_player_encode(v, &b) != RS_ERR_UNSUPPORTED ||
        rs_sync_npc_encode(v, &b) != RS_ERR_UNSUPPORTED ||
        rs_sync_player_decode(v, &b) != RS_ERR_UNSUPPORTED ||
        rs_sync_npc_decode(v, &b) != RS_ERR_UNSUPPORTED) {
        snprintf(msg, sizeof(msg), "rev %d was not RS_ERR_UNSUPPORTED", rev);
        fail(msg);
    }
    rs_sync_view_destroy(v);
}

static void test_solo(int rev, const uint8_t *tele, size_t ntele) {
    rs_sync_view *src;
    rs_sync_view *dst;
    rs_sync_player p;
    rs_sync_player out;
    rs_buf b;
    static const uint8_t idle[] = {0x00, 0x7F, 0xF0};
    static const uint8_t none[] = {0x00};
    src = rs_sync_view_create(rev);
    dst = rs_sync_view_create(rev);
    if (!src || !dst || rs_buf_alloc(&b, 64) != RS_OK) {
        fail("solo alloc");
        rs_sync_view_destroy(src);
        rs_sync_view_destroy(dst);
        return;
    }
    blank_player(&p);
    p.active = 1;
    p.x = 54;
    p.z = 40;
    p.plane = 1;
    p.warp = 1;
    if (rs_sync_set_local(src, 5) != RS_OK || rs_sync_player_set(src, 5, &p) != RS_OK) {
        fail("solo set");
        goto done;
    }
    if (encode_of(src, &b, 0)) {
        goto done;
    }
    expect_mem("local teleport", &b, tele, ntele);
    if (rs_sync_set_local(dst, 5) != RS_OK || decode_of(dst, &b, 0)) {
        goto done;
    }
    if (rs_sync_player_get(dst, 5, &out) != RS_OK || out.x != 54 || out.z != 40 || out.plane != 1 || !out.active) {
        fail("solo round-trip");
        goto done;
    }
    if (encode_of(src, &b, 0)) {
        goto done;
    }
    expect_mem("second tick no movement", &b, idle, sizeof(idle));
    if (encode_of(dst, &b, 0)) {
        goto done;
    }
    expect_mem("decoded view second tick", &b, idle, sizeof(idle));
    if (encode_of(src, &b, 1)) {
        goto done;
    }
    expect_mem("empty npc", &b, none, 1);
done:
    rs_buf_free(&b);
    rs_sync_view_destroy(src);
    rs_sync_view_destroy(dst);
}

static void test_round(int rev, const uint8_t *pb, size_t pn, const uint8_t *nb, size_t nn) {
    rs_sync_view *src;
    rs_sync_view *dst;
    rs_sync_player local;
    rs_sync_player other;
    rs_sync_player out;
    rs_sync_npc npc;
    rs_buf bp;
    rs_buf bn;
    static const uint8_t pidle[] = {0x00, 0xBF, 0xF8};
    memset(&bp, 0, sizeof(bp));
    memset(&bn, 0, sizeof(bn));
    static const uint8_t nidle[] = {0x01, 0x00};
    src = rs_sync_view_create(rev);
    dst = rs_sync_view_create(rev);
    if (!src || !dst || rs_buf_alloc(&bp, 64) != RS_OK || rs_buf_alloc(&bn, 64) != RS_OK) {
        fail("round alloc");
        rs_buf_free(&bp);
        rs_buf_free(&bn);
        rs_sync_view_destroy(src);
        rs_sync_view_destroy(dst);
        return;
    }
    blank_player(&local);
    local.active = 1;
    local.x = 54;
    local.z = 40;
    local.plane = 1;
    local.warp = 1;
    blank_player(&other);
    other.active = 1;
    other.x = 56;
    other.z = 39;
    other.plane = 1;
    other.warp = 1;
    if (rs_sync_set_local(src, 5) != RS_OK || rs_sync_set_local(dst, 5) != RS_OK ||
        rs_sync_player_set(src, 5, &local) != RS_OK || rs_sync_player_set(src, 9, &other) != RS_OK ||
        rs_sync_npc_set(src, 100, 321, 57, 38, 1, 1) != RS_OK) {
        fail("round set");
        goto done;
    }
    if (encode_of(src, &bp, 0) || encode_of(src, &bn, 1)) {
        goto done;
    }
    expect_mem("added player packet", &bp, pb, pn);
    expect_mem("added npc packet", &bn, nb, nn);
    if (decode_of(dst, &bp, 0) || decode_of(dst, &bn, 1)) {
        goto done;
    }
    if (rs_sync_player_get(dst, 5, &out) != RS_OK || out.x != 54 || out.z != 40 || out.plane != 1) {
        fail("local coords");
        goto done;
    }
    if (rs_sync_player_get(dst, 9, &out) != RS_OK || !out.active || out.x != 56 || out.z != 39 || out.plane != 1) {
        fail("added player coords");
        goto done;
    }
    if (rs_sync_npc_get(dst, 100, &npc) != RS_OK || npc.type != 321 || npc.x != 57 || npc.z != 38 || npc.plane != 1) {
        fail("added npc");
        goto done;
    }
    if (encode_of(src, &bp, 0) || encode_of(src, &bn, 1)) {
        goto done;
    }
    expect_mem("second tick player", &bp, pidle, sizeof(pidle));
    expect_mem("second tick npc", &bn, nidle, sizeof(nidle));
    if (encode_of(dst, &bp, 0) || encode_of(dst, &bn, 1)) {
        goto done;
    }
    expect_mem("dest second tick player", &bp, pidle, sizeof(pidle));
    expect_mem("dest second tick npc", &bn, nidle, sizeof(nidle));
    if (rs_sync_player_get(src, 9, &other) != RS_OK) {
        fail("walk get");
        goto done;
    }
    other.dir1 = 1;
    other.dir2 = -1;
    other.run = 0;
    other.warp = 0;
    if (rs_sync_player_set(src, 9, &other) != RS_OK || encode_of(src, &bp, 0) || decode_of(dst, &bp, 0)) {
        goto done;
    }
    if (rs_sync_player_get(dst, 9, &out) != RS_OK || out.x != 56 || out.z != 40) {
        fail("walk step");
        goto done;
    }
done:
    rs_buf_free(&bp);
    rs_buf_free(&bn);
    rs_sync_view_destroy(src);
    rs_sync_view_destroy(dst);
}

static void test_254_hit(void) {
    rs_sync_view *src;
    rs_sync_view *dst;
    rs_sync_player p;
    rs_sync_player out;
    rs_buf b;
    static const uint8_t want[] = {0xEB, 0x65, 0x18, 0x07, 0xFF, 0x10, 0x05, 0x01, 0x0A, 0x14};
    src = rs_sync_view_create(254);
    dst = rs_sync_view_create(254);
    if (!src || !dst || rs_buf_alloc(&b, 64) != RS_OK) {
        fail("254 hit alloc");
        rs_sync_view_destroy(src);
        rs_sync_view_destroy(dst);
        return;
    }
    blank_player(&p);
    p.active = 1;
    p.x = 54;
    p.z = 40;
    p.plane = 1;
    p.warp = 1;
    p.mask = RS_SYNC_P254_HIT;
    p.hit_damage = 5;
    p.hit_type = 1;
    p.hit_hp = 10;
    p.hit_max = 20;
    if (rs_sync_set_local(src, 5) != RS_OK || rs_sync_player_set(src, 5, &p) != RS_OK || encode_of(src, &b, 0)) {
        goto done;
    }
    expect_mem("254 hit", &b, want, sizeof(want));
    if (rs_sync_set_local(dst, 5) != RS_OK || decode_of(dst, &b, 0)) {
        goto done;
    }
    if (rs_sync_player_get(dst, 5, &out) != RS_OK || out.hit_damage != 5 || out.hit_type != 1 ||
        out.hit_hp != 10 || out.hit_max != 20 || out.mask != RS_SYNC_P254_HIT) {
        fail("254 hit round-trip");
    }
done:
    rs_buf_free(&b);
    rs_sync_view_destroy(src);
    rs_sync_view_destroy(dst);
}

static void test_254_look(void) {
    rs_sync_view *src;
    rs_sync_view *dst;
    rs_sync_player p;
    rs_sync_player out;
    rs_buf b;
    src = rs_sync_view_create(254);
    dst = rs_sync_view_create(254);
    if (!src || !dst || rs_buf_alloc(&b, 128) != RS_OK) {
        fail("254 look alloc");
        rs_sync_view_destroy(src);
        rs_sync_view_destroy(dst);
        return;
    }
    fill_look(&p, 254);
    p.mask = RS_SYNC_P254_APPEARANCE;
    p.skill_level = 0x1234;
    p.headicon2 = 0;
    if (rs_sync_set_local(src, 5) != RS_OK || rs_sync_set_local(dst, 5) != RS_OK ||
        rs_sync_player_set(src, 5, &p) != RS_OK || encode_of(src, &b, 0)) {
        goto done;
    }
    /* Teleport bits, mask 0x01, g1 length. Body has no skill u16 (49 bytes). */
    if (b.pos != 56 || b.data[5] != 0x01 || b.data[6] != 49) {
        fail("254 appearance length");
        goto done;
    }
    if (decode_of(dst, &b, 0)) {
        goto done;
    }
    if (rs_sync_player_get(dst, 5, &out) != RS_OK || out.gender != 5 || out.headicon != 4 ||
        out.combat_level != 9 || out.skill_level != 0 || out.styles[0] != 3 || out.styles[1] != 522 ||
        strcmp(out.name, "test") != 0) {
        fail("254 appearance round-trip");
    }
done:
    rs_buf_free(&b);
    rs_sync_view_destroy(src);
    rs_sync_view_destroy(dst);
}

static void test_hit(void) {
    rs_sync_view *src;
    rs_sync_view *dst;
    rs_sync_player p;
    rs_sync_player out;
    rs_buf b;
    static const uint8_t want[] = {0xEE, 0xA1, 0xB0, 0x07, 0xFF, 0x20, 0x05, 0x81, 0xF6, 0x14};
    static const uint8_t idle[] = {0x00, 0x7F, 0xF0};
    src = rs_sync_view_create(317);
    dst = rs_sync_view_create(317);
    if (!src || !dst || rs_buf_alloc(&b, 64) != RS_OK) {
        fail("hit alloc");
        rs_sync_view_destroy(src);
        rs_sync_view_destroy(dst);
        return;
    }
    blank_player(&p);
    p.active = 1;
    p.x = 54;
    p.z = 40;
    p.plane = 1;
    p.warp = 1;
    p.mask = RS_SYNC_P317_HIT;
    p.hit_damage = 5;
    p.hit_type = 1;
    p.hit_hp = 10;
    p.hit_max = 20;
    if (rs_sync_set_local(src, 5) != RS_OK || rs_sync_player_set(src, 5, &p) != RS_OK || encode_of(src, &b, 0)) {
        goto done;
    }
    expect_mem("317 hit", &b, want, sizeof(want));
    if (rs_sync_set_local(dst, 5) != RS_OK || decode_of(dst, &b, 0)) {
        goto done;
    }
    if (rs_sync_player_get(dst, 5, &out) != RS_OK || out.hit_damage != 5 || out.hit_type != 1 ||
        out.hit_hp != 10 || out.hit_max != 20 || out.mask != RS_SYNC_P317_HIT) {
        fail("hit round-trip");
        goto done;
    }
    if (encode_of(src, &b, 0)) {
        goto done;
    }
    expect_mem("hit cleared on next tick", &b, idle, sizeof(idle));
done:
    rs_buf_free(&b);
    rs_sync_view_destroy(src);
    rs_sync_view_destroy(dst);
}

static void test_public(void) {
    rs_sync_view *v;
    rs_sync_player p;
    rs_buf b;
    static const uint8_t want[] = {0xEE, 0xA1, 0xB0, 0x07, 0xFF, 0x80, 0x02, 0x01, 0x03, 0xFD, 0x03, 0x02, 0x01};
    v = rs_sync_view_create(317);
    if (!v || rs_buf_alloc(&b, 64) != RS_OK) {
        fail("public alloc");
        rs_sync_view_destroy(v);
        return;
    }
    blank_player(&p);
    p.active = 1;
    p.x = 54;
    p.z = 40;
    p.plane = 1;
    p.warp = 1;
    p.mask = RS_SYNC_P317_PUBLIC;
    p.chat_colour = 1;
    p.chat_effect = 2;
    p.chat_rights = 3;
    p.packed_len = 3;
    p.packed[0] = 1;
    p.packed[1] = 2;
    p.packed[2] = 3;
    if (rs_sync_set_local(v, 5) != RS_OK || rs_sync_player_set(v, 5, &p) != RS_OK || encode_of(v, &b, 0)) {
        goto done;
    }
    expect_mem("317 public chat", &b, want, sizeof(want));
done:
    rs_buf_free(&b);
    rs_sync_view_destroy(v);
}

static void fill_look(rs_sync_player *p, int rev) {
    int i;
    blank_player(p);
    p->active = 1;
    p->x = 54;
    p->z = 40;
    p->plane = 1;
    p->warp = 1;
    p->gender = 5;
    p->headicon = 4;
    p->headicon2 = rev == 317 ? 0 : -3;
    p->combat_level = 9;
    p->skill_level = 0x1234;
    p->styles[0] = 3;
    p->styles[1] = 522;
    p->colours[0] = 7;
    p->colours[1] = 8;
    p->colours[2] = 9;
    p->colours[3] = 10;
    p->colours[4] = 11;
    for (i = 0; i < 7; i++) {
        p->anims[i] = 10 + i;
    }
    memcpy(p->name, "test", 5);
    if (rev == 317) {
        p->mask = RS_SYNC_P317_APPEARANCE;
    } else if (rev == 350) {
        p->mask = RS_SYNC_P350_APPEARANCE;
    } else {
        p->mask = RS_SYNC_P377_APPEARANCE;
    }
}

static void test_look(int rev, uint8_t mask_byte, const uint8_t *prefix) {
    rs_sync_view *src;
    rs_sync_view *dst;
    rs_sync_player p;
    rs_sync_player out;
    rs_buf b;
    int n;
    int i;
    static const uint8_t idle[] = {0x00, 0x7F, 0xF0};
    src = rs_sync_view_create(rev);
    dst = rs_sync_view_create(rev);
    if (!src || !dst || rs_buf_alloc(&b, 256) != RS_OK) {
        fail("look alloc");
        rs_sync_view_destroy(src);
        rs_sync_view_destroy(dst);
        return;
    }
    fill_look(&p, rev);
    if (rs_sync_set_local(src, 5) != RS_OK || rs_sync_set_local(dst, 5) != RS_OK ||
        rs_sync_player_set(src, 5, &p) != RS_OK || encode_of(src, &b, 0)) {
        goto done;
    }
    if (b.pos < 8 || memcmp(b.data, prefix, 5) != 0 || b.data[5] != mask_byte) {
        expect_mem("appearance prefix", &b, prefix, 5);
        goto done;
    }
    n = (int)b.pos - 7;
    if (rev == 317 && (uint8_t)(-b.data[6]) != (uint8_t)n) {
        fail("317 appearance length is not g1_alt2");
        goto done;
    }
    if (rev == 350 && (uint8_t)(b.data[6] - 128) != (uint8_t)n) {
        fail("350 appearance length is not g1_alt1");
        goto done;
    }
    if (rev == 377 && b.data[6] != (uint8_t)n) {
        fail("377 appearance length is not g1");
        goto done;
    }
    if (rev == 377 && b.data[7] != 0x34) {
        fail("377 appearance body is not reversed");
        goto done;
    }
    if (rev != 377 && b.data[7] != 5) {
        fail("appearance body started with the wrong byte");
        goto done;
    }
    if (decode_of(dst, &b, 0)) {
        goto done;
    }
    if (rs_sync_player_get(dst, 5, &out) != RS_OK || out.gender != 5 || out.skill_level != 0x1234 ||
        out.combat_level != 9 || out.headicon != 4 || out.styles[0] != 3 || out.styles[1] != 522 ||
        strcmp(out.name, "test") != 0) {
        fail("appearance round-trip");
        goto done;
    }
    if (rev != 317 && out.headicon2 != -3) {
        fail("headicon2");
        goto done;
    }
    for (i = 0; i < 5; i++) {
        if (out.colours[i] != 7 + i) {
            fail("colours");
            goto done;
        }
    }
    for (i = 0; i < 7; i++) {
        if (out.anims[i] != 10 + i) {
            fail("anims");
            goto done;
        }
    }
    if (encode_of(src, &b, 0)) {
        goto done;
    }
    expect_mem("appearance not resent", &b, idle, sizeof(idle));
done:
    rs_buf_free(&b);
    rs_sync_view_destroy(src);
    rs_sync_view_destroy(dst);
}

int main(void) {
    static const uint8_t t317[] = {0xEC, 0xA1, 0xB0, 0x07, 0xFF};
    static const uint8_t t350[] = {0xED, 0x94, 0x30, 0x07, 0xFF};
    static const uint8_t t377[] = {0xF5, 0x43, 0x60, 0x07, 0xFF};
    static const uint8_t p317[] = {0xEC, 0xA1, 0xB0, 0x00, 0x09, 0x7E, 0x2F, 0xFE};
    static const uint8_t p350[] = {0xED, 0x94, 0x30, 0x00, 0x09, 0xF8, 0x5F, 0xFE};
    static const uint8_t p377[] = {0xF5, 0x43, 0x60, 0x00, 0x09, 0x13, 0xFF, 0xFE};
    static const uint8_t n317[] = {0x00, 0x01, 0x93, 0xC3, 0x8A, 0x08};
    static const uint8_t n350[] = {0x00, 0x01, 0x92, 0x14, 0x17, 0x86};
    static const uint8_t n377[] = {0x00, 0x01, 0x91, 0xE1, 0xC2, 0x82};
    static const uint8_t a317[] = {0xEE, 0xA1, 0xB0, 0x07, 0xFF};
    static const uint8_t a350[] = {0xED, 0x94, 0x38, 0x07, 0xFF};
    static const uint8_t a377[] = {0xF5, 0x43, 0x68, 0x07, 0xFF};

    static const uint8_t t254[] = {0xEB, 0x65, 0x10, 0x07, 0xFF};
    static const uint8_t p254[] = {0xEB, 0x65, 0x10, 0x00, 0x09, 0x17, 0xEF, 0xFE};
    static const uint8_t n254[] = {0x00, 0x01, 0x90, 0xA0, 0x8F, 0xC0};
    test_refuse(831);
    test_refuse(860);
    test_refuse(910);
    test_solo(254, t254, sizeof(t254));
    test_round(254, p254, sizeof(p254), n254, sizeof(n254));
    test_254_hit();
    test_254_look();
    test_solo(317, t317, sizeof(t317));
    test_solo(350, t350, sizeof(t350));
    test_solo(377, t377, sizeof(t377));
    test_round(317, p317, sizeof(p317), n317, sizeof(n317));
    test_round(350, p350, sizeof(p350), n350, sizeof(n350));
    test_round(377, p377, sizeof(p377), n377, sizeof(n377));
    test_hit();
    test_public();
    test_look(317, 0x10, a317);
    test_look(350, 0x20, a350);
    test_look(377, 0x04, a377);
    return g_fail ? 1 : 0;
}
