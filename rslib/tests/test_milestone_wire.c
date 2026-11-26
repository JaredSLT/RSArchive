#include "rslib.h"

#include <stdio.h>
#include <string.h>

/*
 * One server-side wire check per milestone revision. Region bytes below are
 * the client read order (zone 402,402, scene local 54,54, plane 0, no cache).
 * (402±6)/8 is 49..51, and 402/8 is 50, so the XTEA window is 3*3 squares
 * and not the tutorial-island skip. Nine squares * 16 zero key bytes.
 *
 * sync.c kind_of supports player blocks for 317, 350, and 377 only.
 * codec.h RS_PLAYER_APPEARANCE is the appearance bit of rs_player_update.mask.
 * proto.c rs_handshake_parse looks up opcode 15's revision, so a 317 or
 * 177 hint must still report JS5 revision 474.
 */

static int fails;
static char namebuf[96];

static const char *nm(const char *what, int rev) {
    snprintf(namebuf, sizeof(namebuf), "%s %d", what, rev);
    return namebuf;
}

static void expect(const char *name, int cond) {
    if (!cond) {
        fprintf(stderr, "fail %s\n", name);
        fails++;
    }
}

static void expect_err(const char *name, rs_err got, rs_err want) {
    if (got != want) {
        fprintf(stderr, "fail %s err %d want %d\n", name, (int)got, (int)want);
        fails++;
    }
}

static void dump(const char *name, const rs_buf *b) {
    size_t i;
    fprintf(stderr, "fail %s wire", name);
    for (i = 0; i < b->pos && i < 24; i++) {
        fprintf(stderr, " %02x", b->data[i]);
    }
    if (b->pos > 24) {
        fprintf(stderr, " ... (%u bytes)", (unsigned)b->pos);
    }
    fprintf(stderr, "\n");
    fails++;
}

/* head, then zeron zero bytes, then tail. zeron is the XTEA block. */
static void expect_wire(const char *name, const rs_buf *b, const uint8_t *head, size_t hn,
                         size_t zeron, const uint8_t *tail, size_t tn) {
    size_t i;
    size_t need = hn + zeron + tn;
    if (b->pos != need || (hn && memcmp(b->data, head, hn) != 0) ||
        (tn && memcmp(b->data + hn + zeron, tail, tn) != 0)) {
        dump(name, b);
        return;
    }
    for (i = 0; i < zeron; i++) {
        if (b->data[hn + i] != 0) {
            dump(name, b);
            return;
        }
    }
}

/* rslib/src/both/sync.c kind_of. 350 is not in this milestone list. */
static int sync_player(int rev) {
    return rev == 254 || rev == 317 || rev == 350 || rev == 377;
}

static void session_of(rs_session *s, int rev) {
    const rs_build_info *info = rs_build_lookup(rev);
    memset(s, 0, sizeof(*s));
    rs_session_init_role(s, info, RS_ROLE_SERVER);
}

static int region_supported(const rs_session *s) {
    rs_region z;
    rs_buf b;
    uint8_t raw[256];
    memset(&z, 0, sizeof(z));
    rs_buf_wrap(&b, raw, sizeof(raw));
    return rs_codec_region_encode(s, &b, &z) == RS_OK;
}

/*
 * Client layouts for zone 402,402 / local 54,54 / plane 0 / null cache.
 * Returns 0 when the decompiled reader was not pinned (round-trip only).
 */
static int region_expect(int rev, const uint8_t **head, size_t *hn, size_t *zeron,
                          const uint8_t **tail, size_t *tn) {
    /* Client.java ptype 209: g2(zoneX), g2(zoneZ). */
    static const uint8_t r254[] = {0x01, 0x92, 0x01, 0x92};
    /* Client.java packet 73: g2_alt2(zoneX), g2(zoneZ). Packet.g2_alt2. */
    static const uint8_t r317[] = {0x01, 0x12, 0x01, 0x92};
    /* Client.java opcode 222: g2(zoneZ), g2_alt3(zoneX). Packet.g2_alt3. */
    static const uint8_t r377[] = {0x01, 0x92, 0x12, 0x01};
    /*
     * ByteArrayNodeSecondary.opByteArrayNodeSecondary24 normal branch.
     * g2_alt3(local), g1(plane), g2(zoneX), LE keys, g2_alt2(local), g2(zoneZ).
     * Js5CacheRequest.opJs5CacheRequest43 stores the first local as Z and the
     * second as X. Both are 54, so the two transforms are not distinguished.
     */
    static const uint8_t r474_h[] = {0xb6, 0x00, 0x00, 0x01, 0x92};
    static const uint8_t r474_t[] = {0x00, 0xb6, 0x01, 0x92};
    /*
     * Js5Index.opJs5Index5 normal branch, MapChunk getters.
     * g2_alt2(zoneX), g2_alt3(local), g2_alt2(local), BE keys, g1_neg(plane), g2(zoneZ).
     * TextureOp29SubOp4.opTextureOp29SubOp413: first local argument is Z, second is X.
     * Both are 54.
     */
    static const uint8_t r508_h[] = {0x01, 0x12, 0xb6, 0x00, 0x00, 0xb6};
    static const uint8_t r508_t[] = {0x00, 0x01, 0x92};
    /*
     * C633b3b0d.opkind5884(false), Unused6 getters.
     * g2_alt2(localX), alt3 keys, g1_alt3(plane), g2(zoneX), g2_alt2(zoneZ), g2_alt2(localZ).
     */
    static const uint8_t r530_h[] = {0x00, 0xb6};
    static const uint8_t r530_t[] = {0x80, 0x01, 0x92, 0x01, 0x12, 0x00, 0xb6};
    /*
     * World.rebuildNormalMap. Packet g2_alt2 / g1 / g1_alt3.
     * g2_alt2(zoneX), g1(npc view 14), g1_alt3(window count 9), g1(build area 0),
     * g2_alt2(zoneZ), g1_alt3(force reload 0). Standard build area walks
     * (zone ± mapSize>>4) / 8 with mapSize 104, which is the same ±6 window.
     */
    static const uint8_t r910[] = {0x01, 0x12, 0x0e, 0x77, 0x00, 0x01, 0x12, 0x80};

    *head = NULL;
    *hn = 0;
    *zeron = 0;
    *tail = NULL;
    *tn = 0;
    switch (rev) {
    case 254:
        *head = r254;
        *hn = sizeof(r254);
        return 1;
    case 317:
        *head = r317;
        *hn = sizeof(r317);
        return 1;
    case 377:
        *head = r377;
        *hn = sizeof(r377);
        return 1;
    case 468:
        /* 468 through 507 use the 474 rebuild. The bytes match that pin. */
        *head = r474_h;
        *hn = sizeof(r474_h);
        *zeron = 144;
        *tail = r474_t;
        *tn = sizeof(r474_t);
        return 1;
    case 474:
        *head = r474_h;
        *hn = sizeof(r474_h);
        *zeron = 144;
        *tail = r474_t;
        *tn = sizeof(r474_t);
        return 1;
    case 508:
        *head = r508_h;
        *hn = sizeof(r508_h);
        *zeron = 144;
        *tail = r508_t;
        *tn = sizeof(r508_t);
        return 1;
    case 530:
        *head = r530_h;
        *hn = sizeof(r530_h);
        *zeron = 144;
        *tail = r530_t;
        *tn = sizeof(r530_t);
        return 1;
    case 910:
        *head = r910;
        *hn = sizeof(r910);
        return 1;
    default:
        /*
         * No pinned bytes. 115/177 MudClient has no two-u16 zone rebuild.
         * 614 C0bd9f077.opkind11111 and 667 C7a6fda97.opkind20206 carry an
         * XTEA window whose length depends on a build-area table, so the
         * body is not invented here. 742 has no readable rebuild reader.
         */
        return 0;
    }
}

static void test_region(const rs_session *s, int rev, int cap_region) {
    rs_region in;
    rs_region out;
    rs_buf b;
    rs_buf dec;
    uint8_t raw[2048];
    const uint8_t *head;
    const uint8_t *tail;
    size_t hn, zeron, tn;
    rs_err e;

    memset(&in, 0, sizeof(in));
    in.zone_x = 402;
    in.zone_z = 402;
    in.local_x = 54;
    in.local_z = 54;
    rs_buf_wrap(&b, raw, sizeof(raw));
    e = rs_codec_region_encode(s, &b, &in);
    if (e == RS_ERR_UNSUPPORTED) {
        expect(nm("region cap off", rev), cap_region == 0);
        return;
    }
    expect_err(nm("region encode", rev), e, RS_OK);
    if (e != RS_OK) {
        return;
    }
    if (region_expect(rev, &head, &hn, &zeron, &tail, &tn)) {
        expect_wire(nm("region bytes", rev), &b, head, hn, zeron, tail, tn);
    }
    rs_buf_wrap(&dec, b.data, b.pos);
    e = rs_codec_region_decode(s, &dec, &out);
    expect_err(nm("region decode", rev), e, RS_OK);
    if (e == RS_OK) {
        expect(nm("region zone", rev), out.zone_x == 402 && out.zone_z == 402);
    }
}

static void test_player(const rs_session *s, int rev, int cap_player) {
    rs_player_update up;
    rs_player_update out;
    rs_buf b;
    rs_buf dec;
    uint8_t raw[512];
    rs_err e;

    memset(&up, 0, sizeof(up));
    up.teleport = 1;
    up.local_x = 54;
    up.local_z = 54;
    up.include_appearance = 1;
    up.mask = RS_PLAYER_APPEARANCE;
    rs_buf_wrap(&b, raw, sizeof(raw));
    e = rs_codec_player_update_encode(s, &b, &up);
    if (!cap_player) {
        expect_err(nm("player unsupported", rev), e, RS_ERR_UNSUPPORTED);
        return;
    }
    expect_err(nm("player encode", rev), e, RS_OK);
    if (e != RS_OK) {
        return;
    }
    expect(nm("player length", rev), b.pos > 0);
    rs_buf_wrap(&dec, b.data, b.pos);
    e = rs_codec_player_update_decode(s, &dec, &out);
    if (e == RS_ERR_UNSUPPORTED) {
        return;
    }
    expect_err(nm("player decode", rev), e, RS_OK);
    if (e == RS_OK) {
        expect(nm("player round trip", rev),
               out.teleport == 1 && out.local_x == 54 && out.local_z == 54);
    }
}

static void test_skill(const rs_session *s, int rev) {
    rs_era_skill esk;
    rs_skill sk;
    rs_buf era;
    rs_buf codec;
    uint8_t era_raw[32];
    uint8_t codec_raw[32];
    rs_err era_e;
    rs_err codec_e;

    memset(&esk, 0, sizeof(esk));
    memset(&sk, 0, sizeof(sk));
    esk.id = 0;
    esk.level = 1;
    esk.xp = 0;
    sk.id = 0;
    sk.level = 1;
    sk.xp = 0;
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    codec_e = rs_codec_skill_encode(s, &codec, &sk);
    /* 474 and 530 must not fall back to the 317 skill body. */
    if (rev == 474 || rev == 530) {
        expect_err(nm("skill unsupported", rev), codec_e, RS_ERR_UNSUPPORTED);
        return;
    }
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    era_e = rs_era_skill_encode(rev, &era, &esk);
    if (era_e == RS_OK) {
        expect_err(nm("skill codec", rev), codec_e, RS_OK);
        if (codec_e == RS_OK &&
            (codec.pos != era.pos || memcmp(codec.data, era.data, era.pos) != 0)) {
            dump(nm("skill bytes", rev), &codec);
        }
        return;
    }
    expect(nm("skill result", rev), codec_e == RS_OK || codec_e == RS_ERR_UNSUPPORTED);
    if (codec_e == RS_OK) {
        expect(nm("skill length", rev), codec.pos > 0);
    }
}

static void test_revision(int rev) {
    const rs_build_info *info = rs_build_lookup(rev);
    rs_build_caps caps;
    rs_session s;
    int want_frame;
    int want_player;
    int want_region;

    expect(nm("lookup", rev), info != NULL && info->revision == rev);
    if (!info) {
        return;
    }
    memset(&caps, 0, sizeof(caps));
    rs_build_capabilities(info, &caps);
    /* sizes_s2c rows frame s2c. RSC 115 and 177 frame both ways with no table. */
    want_frame = (info->sizes_s2c != NULL || info->era == RS_ERA_RSC) ? 1 : 0;
    want_player = sync_player(rev) ? 1 : 0;
    session_of(&s, rev);
    want_region = region_supported(&s) ? 1 : 0;
    expect(nm("frame_s2c", rev), caps.frame_s2c == want_frame && want_frame == 1);
    expect(nm("player_sync", rev), caps.player_sync == want_player);
    expect(nm("region cap", rev), caps.region == want_region);
    test_region(&s, rev, caps.region);
    test_player(&s, rev, caps.player_sync);
    test_skill(&s, rev);
}

/* Client.java login(): p1(14), p1(name hash). */
static void test_login_handshake(void) {
    const rs_build_info *info = rs_build_lookup(317);
    rs_session client;
    rs_session server;
    rs_buf b;
    rs_handshake hs;
    uint8_t raw[16];
    rs_err e;
    static const uint8_t want[] = {14, 32};

    memset(&client, 0, sizeof(client));
    memset(&server, 0, sizeof(server));
    rs_session_init_role(&client, info, RS_ROLE_CLIENT);
    rs_session_init_role(&server, info, RS_ROLE_SERVER);
    rs_buf_wrap(&b, raw, sizeof(raw));
    e = rs_session_handshake_write(&client, &b, RS_LINK_LOGIN, 32);
    expect_err("317 login write", e, RS_OK);
    if (e == RS_OK) {
        expect_wire("317 login bytes", &b, want, sizeof(want), 0, NULL, 0);
        e = rs_session_handshake_parse(&server, raw, b.pos, &hs);
        expect_err("317 login parse", e, RS_OK);
        expect("317 login link", e == RS_OK && hs.link == RS_LINK_LOGIN && hs.name_hash == 32);
    }
}

/* js5_handshake_write: opcode 15 and a big-endian revision. */
static void test_js5_handshake(void) {
    const rs_build_info *info = rs_build_lookup(474);
    rs_session client;
    rs_session server;
    rs_buf b;
    rs_handshake hs;
    uint8_t raw[16];
    rs_err e;
    static const uint8_t want[] = {15, 0, 0, 1, 218};

    memset(&client, 0, sizeof(client));
    memset(&server, 0, sizeof(server));
    rs_session_init_role(&client, info, RS_ROLE_CLIENT);
    rs_session_init_role(&server, info, RS_ROLE_SERVER);
    rs_buf_wrap(&b, raw, sizeof(raw));
    e = rs_session_handshake_write(&client, &b, RS_LINK_JS5, 0);
    expect_err("474 js5 write", e, RS_OK);
    if (e != RS_OK) {
        return;
    }
    expect_wire("474 js5 bytes", &b, want, sizeof(want), 0, NULL, 0);
    e = rs_session_handshake_parse(&server, raw, b.pos, &hs);
    expect_err("474 js5 parse", e, RS_OK);
    expect("474 js5 revision", e == RS_OK && hs.link == RS_LINK_JS5 && hs.revision == 474);
}

/*
 * Opcode 15 carries the revision. A non-JS5 hint must not hide it.
 * Opcode 14 is login for an RS2 hint and may also be login for an RSC hint.
 */
static void test_handshake_classify(void) {
    const rs_build_info *b317 = rs_build_lookup(317);
    const rs_build_info *b177 = rs_build_lookup(177);
    rs_handshake hs;
    rs_err e;
    static const uint8_t js5[] = {15, 0, 0, 1, 218};
    static const uint8_t login[] = {14, 32};

    e = rs_handshake_parse(b317, js5, sizeof(js5), &hs);
    expect_err("hint 317 opcode 15", e, RS_OK);
    expect("hint 317 is js5 474", e == RS_OK && hs.link == RS_LINK_JS5 && hs.revision == 474);

    e = rs_handshake_parse(b177, js5, sizeof(js5), &hs);
    expect_err("hint 177 opcode 15", e, RS_OK);
    expect("hint 177 is js5 474", e == RS_OK && hs.link == RS_LINK_JS5 && hs.revision == 474);

    e = rs_handshake_parse(b317, login, sizeof(login), &hs);
    expect_err("hint 317 opcode 14", e, RS_OK);
    expect("hint 317 is login", e == RS_OK && hs.link == RS_LINK_LOGIN && hs.name_hash == 32);

    e = rs_handshake_parse(b177, login, sizeof(login), &hs);
    expect_err("hint 177 opcode 14", e, RS_OK);
    expect("hint 177 is login", e == RS_OK && hs.link == RS_LINK_LOGIN);
}

int main(void) {
    static const int revs[] = {115, 177, 254, 317, 377, 468, 474, 508, 530, 614, 667, 742, 910};
    size_t i;
    for (i = 0; i < sizeof(revs) / sizeof(revs[0]); i++) {
        test_revision(revs[i]);
    }
    test_login_handshake();
    test_js5_handshake();
    test_handshake_classify();
    return fails ? 1 : 0;
}
