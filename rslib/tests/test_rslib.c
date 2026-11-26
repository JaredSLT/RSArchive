#include "rslib.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static int g_fail;
static int g_got_login;
static int g_got_walk;
static int g_got_client_login;
static int g_got_region;
static int g_got_player;
static int g_got_message;
static rs_conn *g_login_conn;

static const rs_codec_ops *codec_for_rev(int rev) {
    switch (rev) {
    case 115:
    case 159:
    case 177:
    case 443:
    case 525:
    case 578:
    case 614:
    case 634:
    case 667:
    case 695:
    case 742:
    case 745:
        return &rs_codec_plain;
    case 254:
    case 282:
    case 317:
    case 350:
    case 377:
        return &rs_codec_rs2;
    case 468:
    case 474:
    case 481:
        return &rs_codec_js5_474;
    case 508:
        return &rs_codec_js5_508;
    case 530:
        return &rs_codec_js5_530;
    case 790:
    case 831:
    case 860:
    case 876:
    case 887:
    case 902:
    case 910:
        return &rs_codec_rs3;
    default:
        return NULL;
    }
}

static void put_be16(uint8_t *p, int v) {
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static void put_be24(uint8_t *p, int v) {
    p[0] = (uint8_t)(v >> 16);
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)v;
}

static void put_be32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static void test_on_login(rs_conn *c, const rs_login_request *login) {
    (void)login;
    g_got_login = 1;
    g_login_conn = c;
    rs_conn_accept_login(c, 0, 0);
}

static void test_on_client_packet(rs_client *c, rs_pkt id, int opcode, rs_buf *payload) {
    (void)c;
    (void)opcode;
    (void)payload;
    if (id == RS_S2C_REGION) {
        g_got_region = 1;
    }
    if (id == RS_S2C_PLAYER_UPDATE) {
        g_got_player = 1;
    }
    if (id == RS_S2C_MESSAGE) {
        g_got_message = 1;
    }
}

static void test_on_packet(rs_conn *c, rs_pkt id, int opcode, rs_buf *payload) {
    (void)c;
    (void)opcode;
    (void)payload;
    if (id == RS_C2S_WALK) {
        g_got_walk = 1;
    }
}

static void test_on_client_login(rs_client *c, int status, int rights, int flagged) {
    (void)c;
    (void)rights;
    (void)flagged;
    if (status == RS_LOGIN_OK || status == 0) {
        g_got_client_login = 1;
    }
}

static void expect(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        g_fail++;
    }
}

static void expect_err(rs_err e, rs_err want, const char *msg) {
    if (e != want) {
        fprintf(stderr, "FAIL: %s (got %s)\n", msg, rs_strerror(e));
        g_fail++;
    }
}

#ifndef RSARCHIVE_ROOT
#define RSARCHIVE_ROOT "."
#endif

static int can_send(const rs_session *s, rs_pkt id, size_t n) {
    int op, sz;
    if (!s) {
        return 0;
    }
    op = rs_session_pkt_opcode(s, id);
    if (op < 0) {
        return 0;
    }
    sz = rs_session_size_out(s, op);
    if (sz == -1 || sz == -2) {
        return 1;
    }
    return sz >= 0 && (size_t)sz == n;
}

static void loopback_build(const char *id) {
    rs_server_hooks sh;
    rs_client_hooks ch;
    rs_ctx *sctx = rs_ctx_create();
    rs_ctx *cctx = rs_ctx_create();
    rs_server *srv;
    rs_client *cli;
    int i, port;
    char msg[96];
    int want_region, want_player, want_message, want_walk;

    memset(&sh, 0, sizeof(sh));
    memset(&ch, 0, sizeof(ch));
    g_got_login = g_got_walk = g_got_client_login = 0;
    g_got_region = g_got_player = g_got_message = 0;
    g_login_conn = NULL;
    sh.on_login = test_on_login;
    sh.on_packet = test_on_packet;
    ch.on_login = test_on_client_login;
    ch.on_packet = test_on_client_packet;
    snprintf(msg, sizeof(msg), "%s set build", id);
    expect_err(rs_ctx_set_build_id(sctx, id), RS_OK, msg);
    expect_err(rs_ctx_set_build_id(cctx, id), RS_OK, msg);
    srv = rs_server_create(sctx, &sh);
    cli = rs_client_create(cctx, &ch);
    snprintf(msg, sizeof(msg), "%s loopback create", id);
    expect(srv && cli, msg);
    snprintf(msg, sizeof(msg), "%s listen", id);
    expect_err(rs_server_listen(srv, "127.0.0.1", 0), RS_OK, msg);
    port = rs_server_port(srv);
    snprintf(msg, sizeof(msg), "%s ephemeral port", id);
    expect(port > 0, msg);
    snprintf(msg, sizeof(msg), "%s connect", id);
    expect_err(rs_client_connect(cli, "127.0.0.1", port), RS_OK, msg);
    for (i = 0; i < 80 && !g_got_client_login; i++) {
        rs_server_poll(srv, 0);
        rs_client_poll(cli, 0);
        if (i == 2) {
            rs_client_handshake(cli, RS_LINK_LOGIN, 7);
        }
        if (i == 12) {
            rs_client_login(cli, "player1", "pass", 0);
        }
    }
    snprintf(msg, sizeof(msg), "%s server saw login", id);
    expect(g_got_login, msg);
    snprintf(msg, sizeof(msg), "%s client saw login-ok", id);
    expect(g_got_client_login, msg);
    want_region = want_player = want_message = want_walk = 0;
    if (g_got_client_login && g_login_conn) {
        uint8_t raw[512];
        rs_buf b;
        rs_region region;
        rs_player_update pu;
        rs_message m;
        rs_session *ss = rs_conn_session(g_login_conn);
        memset(&region, 0, sizeof(region));
        region.zone_x = 402;
        region.zone_z = 402;
        region.local_x = 54;
        region.local_z = 54;
        rs_buf_wrap(&b, raw, sizeof(raw));
        if (rs_codec_region_encode(ss, &b, &region) == RS_OK && can_send(ss, RS_S2C_REGION, b.pos)) {
            want_region = 1;
            rs_conn_send_pkt(g_login_conn, RS_S2C_REGION, raw, b.pos);
        }
        memset(&pu, 0, sizeof(pu));
        pu.teleport = 1;
        pu.include_appearance = 1;
        pu.local_x = 54;
        pu.local_z = 54;
        rs_buf_wrap(&b, raw, sizeof(raw));
        if (rs_codec_player_update_encode(ss, &b, &pu) == RS_OK &&
            can_send(ss, RS_S2C_PLAYER_UPDATE, b.pos)) {
            want_player = 1;
            rs_conn_send_pkt(g_login_conn, RS_S2C_PLAYER_UPDATE, raw, b.pos);
        }
        memset(&m, 0, sizeof(m));
        strcpy(m.text, "Welcome to rslib-world.");
        rs_buf_wrap(&b, raw, sizeof(raw));
        if (rs_codec_message_encode(ss, &b, &m) == RS_OK && can_send(ss, RS_S2C_MESSAGE, b.pos)) {
            want_message = 1;
            rs_conn_send_pkt(g_login_conn, RS_S2C_MESSAGE, raw, b.pos);
        }
        for (i = 0; i < 30 && !((!want_region || g_got_region) && (!want_player || g_got_player) &&
                                (!want_message || g_got_message));
             i++) {
            rs_server_poll(srv, 0);
            rs_client_poll(cli, 0);
        }
        if (want_region) {
            snprintf(msg, sizeof(msg), "%s client saw region", id);
            expect(g_got_region, msg);
        }
        if (want_player) {
            snprintf(msg, sizeof(msg), "%s client saw player update", id);
            expect(g_got_player, msg);
        }
        if (want_message) {
            snprintf(msg, sizeof(msg), "%s client saw message", id);
            expect(g_got_message, msg);
        }
    }
    if (g_got_client_login) {
        uint8_t walk[32];
        rs_buf b;
        rs_walk w;
        rs_session *cs = rs_client_session(cli);
        memset(&w, 0, sizeof(w));
        w.dest_x = 3222;
        w.dest_z = 3222;
        rs_buf_wrap(&b, walk, sizeof(walk));
        if (rs_codec_walk_encode(cs, &b, &w) == RS_OK && can_send(cs, RS_C2S_WALK, b.pos)) {
            want_walk = 1;
            rs_client_send_pkt(cli, RS_C2S_WALK, walk, b.pos);
            for (i = 0; i < 30 && !g_got_walk; i++) {
                rs_server_poll(srv, 0);
                rs_client_poll(cli, 0);
            }
        }
        if (want_walk) {
            snprintf(msg, sizeof(msg), "%s server saw walk", id);
            expect(g_got_walk, msg);
        }
    }
    rs_client_destroy(cli);
    rs_server_destroy(srv);
    rs_ctx_destroy(sctx);
    rs_ctx_destroy(cctx);
}

int main(void) {
    rs_ctx *ctx;
    size_t nbuilds = 0;
    const rs_build_info *all;
    uint32_t seed[4] = {1, 2, 3, 4};
    rs_isaac a, b;
    rs_buf buf;
    uint8_t scratch[64];
    uint32_t xkey[4] = {1, 2, 3, 4};
    uint8_t block[8] = {9, 8, 7, 6, 5, 4, 3, 2};
    uint8_t orig[8];
    char name[13];
    rs_session sess;
    rs_handshake hs;
    uint8_t hsbuf[8];
    rs_build_info custom;

    all = rs_build_known(&nbuilds);
    expect(nbuilds == 29, "known builds include every archived client");
    expect(rs_build_lookup(317) == &all[4] || rs_build_lookup(317)->revision == 317, "lookup 317");
    expect(rs_build_lookup_id("2005-317") && rs_build_lookup_id("2005-317")->revision == 317,
           "lookup id 2005-317");
    expect(rs_build_lookup_id("377") && rs_build_lookup_id("377")->revision == 377, "lookup 377");
    expect(rs_build_lookup_id("2012-742")->era == RS_ERA_JS5, "742 is JS5");
    expect(rs_build_lookup(115)->cache_kind == RS_CACHE_JAG, "115 is jag");
    expect(rs_build_lookup(317)->ondemand && !rs_build_lookup(317)->js5, "317 ondemand");
    expect(rs_build_lookup(474)->js5 && rs_build_lookup(474)->xtea_maps, "474 js5+xtea");

    custom = *rs_build_lookup(317);
    custom.revision = 318;
    custom.id = "test-318";
    custom.name = "custom 318";
    expect_err(rs_build_register(&custom), RS_OK, "register custom build");
    expect(rs_build_lookup(318) && rs_build_lookup(318)->revision == 318, "custom lookup");

    ctx = rs_ctx_create();
    expect(ctx != NULL, "ctx create");
    {
        rs_client *cli = rs_client_create(ctx, NULL);
        expect(cli != NULL, "client create");
        expect(rs_client_session(cli) && rs_client_session(cli)->role == RS_ROLE_CLIENT, "client role");
        rs_client_destroy(cli);
        {
            rs_server *srv = rs_server_create(ctx, NULL);
            expect(srv != NULL, "server create");
            rs_server_destroy(srv);
        }
    }
    expect_err(rs_ctx_set_build_id(ctx, "317"), RS_OK, "set build 317");
    expect(rs_ctx_build(ctx)->revision == 317, "ctx build");
    expect_err(rs_ctx_set_build_revision(ctx, 474), RS_OK, "switch ctx to 474");
    expect(rs_ctx_build(ctx)->revision == 474, "ctx switched");
    expect_err(rs_ctx_set_build_id(ctx, "nope"), RS_ERR_VERSION, "unknown build");

    expect(rs_crc32(NULL, 0) == 0, "crc32 empty");
    expect(rs_crc32("123456789", 9) == 0xCBF43926u, "crc32 ieee");

    memcpy(orig, block, 8);
    rs_xtea_encrypt(xkey, block, 8);
    expect(memcmp(block, orig, 8) != 0, "xtea mutates");
    rs_xtea_decrypt(xkey, block, 8);
    expect(memcmp(block, orig, 8) == 0, "xtea roundtrip");

    rs_isaac_init(&a, seed);
    rs_isaac_init(&b, seed);
    expect(rs_isaac_next(&a) == rs_isaac_next(&b), "isaac deterministic");
    {
        uint32_t k[4] = {10, 20, 30, 40};
        rs_isaac client_out;
        int op = 164, enc;
        rs_isaac_init(&client_out, k);
        rs_session_init(&sess, rs_build_lookup(317));
        rs_session_seed_isaac(&sess, k);
        enc = (int)((op + rs_isaac_next(&client_out)) & 0xff);
        expect(rs_session_decode_opcode(&sess, enc) == op, "isaac opcode roundtrip");
    }

    expect_err(rs_buf_alloc(&buf, 32), RS_OK, "buf alloc");
    expect_err(rs_p1(&buf, 0xAB), RS_OK, "p1");
    expect_err(rs_p2(&buf, 0x1234), RS_OK, "p2");
    expect_err(rs_p4(&buf, 0x89ABCDEF), RS_OK, "p4");
    expect_err(rs_pjstr(&buf, "hi"), RS_OK, "pjstr");
    buf.pos = 0;
    expect(rs_g1(&buf) == 0xAB, "g1");
    expect(rs_g2(&buf) == 0x1234, "g2");
    expect((uint32_t)rs_g4(&buf) == 0x89ABCDEFu, "g4");
    expect_err(rs_gjstr(&buf, (char *)scratch, sizeof(scratch)), RS_OK, "gjstr");
    expect(strcmp((char *)scratch, "hi") == 0, "gjstr value");
    rs_buf_free(&buf);

    expect_err(rs_buf_alloc(&buf, 16), RS_OK, "bit buf");
    rs_access_bits(&buf);
    expect_err(rs_pbit(&buf, 1, 1), RS_OK, "pbit");
    expect_err(rs_pbit(&buf, 3, 5), RS_OK, "pbit3");
    expect_err(rs_pbit(&buf, 12, 0xABC), RS_OK, "pbit12");
    rs_access_bytes(&buf);
    buf.pos = 0;
    rs_access_bits(&buf);
    expect(rs_gbit(&buf, 1) == 1, "gbit 1");
    expect(rs_gbit(&buf, 3) == 5, "gbit 3");
    expect(rs_gbit(&buf, 12) == 0xABC, "gbit 12");
    rs_buf_free(&buf);

    expect(rs_base37_encode("player1") != 0, "base37 encode");
    rs_base37_decode(rs_base37_encode("player1"), name);
    expect(strcmp(name, "player1") == 0, "base37 roundtrip");

    expect(rs_hash_js5("l40_55") == (int32_t)-1153472937, "js5 name hash from keys.json");

    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_WALK) == 164, "317 walk");
    {
        rs_session s317;
        rs_session_init(&s317, rs_build_lookup(317));
        expect(rs_session_pkt_opcode(&s317, RS_C2S_WALK) == 164, "session walk opcode");
        expect(rs_session_pkt_id(&s317, 164) == RS_C2S_WALK, "session walk id");
    }
    expect(rs_pkt_opcode(rs_build_lookup(254), RS_C2S_WALK) == 6, "254 walk");
    expect(rs_pkt_opcode(rs_build_lookup(377), RS_C2S_WALK) == 28, "377 walk");
    expect(rs_pkt_opcode(rs_build_lookup(742), RS_C2S_WALK) == 74, "742 walk");
    expect(rs_pkt_opcode(rs_build_lookup(468), RS_C2S_WALK) == 184, "468 walk");
    expect(rs_pkt_opcode(rs_build_lookup(474), RS_C2S_WALK) == 46, "474 walk");
    expect(rs_pkt_opcode(rs_build_lookup(508), RS_C2S_WALK) == 49, "508 walk");
    expect(rs_pkt_opcode(rs_build_lookup(530), RS_C2S_WALK) == 215, "530 walk");
    expect(rs_build_lookup(468) && rs_build_lookup(468)->js5, "468 is JS5");
    expect(rs_build_lookup(468)->sizes_s2c != NULL && rs_build_lookup(468)->sizes_c2s != NULL,
           "468 sizes");
    expect(rs_pkt_from_opcode(rs_build_lookup(317), RS_DIR_C2S, 164) == RS_C2S_WALK, "317 walk reverse");
    expect_err(rs_pkt_copy(318, 317), RS_OK, "copy 317 -> 318");
    {
        rs_build_info fake = *rs_build_lookup(317);
        fake.revision = 318;
        expect(rs_pkt_opcode(&fake, RS_C2S_WALK) == 164, "318 walk after copy");
    }
    expect(rs_build_lookup(254)->sizes_c2s != NULL, "254 c2s sizes");
    expect(rs_build_lookup(377)->sizes_c2s != NULL, "377 c2s sizes");
    expect(rs_build_lookup(474)->sizes_s2c != NULL && rs_build_lookup(474)->sizes_c2s != NULL, "474 sizes");
    expect(rs_build_lookup(508)->sizes_c2s != NULL, "508 c2s");
    expect(rs_build_lookup(530)->sizes_c2s != NULL, "530 c2s");
    expect(rs_build_lookup(614)->sizes_c2s != NULL && rs_build_lookup(614)->sizes_s2c != NULL, "614 sizes");
    expect(rs_build_lookup(667)->sizes_c2s != NULL && rs_build_lookup(667)->sizes_s2c != NULL, "667 sizes");
    expect(rs_build_lookup(667)->js5_index_format == 6 && rs_build_lookup(667)->store_archives == 37 &&
               rs_build_lookup(667)->login_crc_count == 37,
           "667 protocol numbers unchanged");
    expect(rs_build_lookup(508)->login_crc_count == 29 && rs_build_lookup(508)->store_archives == 29,
           "508 protocol numbers unchanged");
    expect(rs_build_lookup(742)->login_crc_count == 40 && rs_build_lookup(742)->store_archives == 40 &&
               rs_build_lookup(742)->js5_index_format == 7,
           "742 protocol numbers unchanged");
    expect(rs_build_lookup(282)->era == RS_ERA_RS2 && rs_build_lookup(282)->ondemand &&
               rs_build_lookup(282)->sizes_s2c != NULL && rs_build_lookup(282)->sizes_c2s == NULL,
           "282 old cache, s2c only");
    expect(rs_build_lookup(443)->js5 && rs_build_lookup(443)->store_archives == 14 &&
               rs_build_lookup(443)->js5_index_format == 5 && rs_build_lookup(443)->login_crc_count == 14,
           "443 js5");
    expect(rs_build_lookup(481)->login_user37 == 0 && rs_build_lookup(481)->login_player_index == 1 &&
               rs_build_lookup(481)->login_success_extra == 0,
           "481 login flags");
    expect(rs_build_lookup(525)->store_archives == 28 && rs_build_lookup(525)->login_success_extra == 1,
           "525 store and extra");
    expect(rs_build_lookup(578)->sizes_s2c == NULL && rs_build_lookup(578)->sizes_c2s == NULL &&
               rs_build_lookup(578)->large_file_ids == 0,
           "578 has no size table");
    expect(rs_build_lookup(634)->large_file_ids == 1 && rs_build_lookup(634)->store_archives == 37 &&
               rs_build_lookup(634)->sizes_s2c == NULL,
           "634 large ids, no sizes");
    expect(rs_build_lookup(695)->js5_index_format == 7 && rs_build_lookup(695)->sizes_s2c == NULL &&
               rs_build_lookup(695)->store_archives == 37,
           "695 registered without sizes");
    expect(rs_build_lookup(745)->store_archives == 38 && rs_build_lookup(745)->login_crc_count == 37 &&
               rs_build_lookup(745)->sizes_c2s != NULL,
           "745 archive span");
    expect(rs_build_lookup(790) && strcmp(rs_build_lookup(790)->version, "rs3") == 0, "790 is rs3");
    expect(rs_build_lookup(831)->sizes_s2c == NULL && rs_build_lookup(860)->sizes_c2s == NULL,
           "831 and 860 have no sources");
    expect(rs_build_lookup(910)->login_crc_count == 41 && rs_build_lookup(910)->store_archives == 57,
           "910 crc and stores");
    expect(rs_pkt_opcode(rs_build_lookup(910), RS_C2S_WALK) == 33, "910 walk");
    expect(rs_pkt_opcode(rs_build_lookup(910), RS_S2C_REGION) == 88, "910 rebuild normal");
    expect(rs_pkt_opcode(rs_build_lookup(910), RS_S2C_MESSAGE) == 120, "910 game message");
    {
        size_t nb = 0, bi;
        const rs_build_info *known = rs_build_known(&nb);
        static const int no_s2c[] = {578, 634, 695, 831, 860};
        static const int no_c2s[] = {282, 350, 443, 481, 525, 578, 634, 695, 831, 860};
        int prev = 0;
        expect(nb == 29, "29 archived client snapshots");
        for (bi = 0; bi < nb; bi++) {
            const rs_build_info *b = &known[bi];
            char rel[160];
            char expect_rel[160];
            int i, skip_s2c = 0, skip_c2s = 0;
            expect(b->revision > prev, "builds are chronological");
            prev = b->revision;
            expect(b->codec == codec_for_rev(b->revision) && rs_codec_for(b) == b->codec,
                   "codec delegate");
            if (b->revision >= 790) {
                expect(b->ops == &rs_ops_js5_lobby && rs_proto_for(b) == rs_proto_js5_lobby(),
                       "rs3 lobby delegate");
            } else {
                expect(b->ops == NULL, "non-rs3 ops stay on the era");
            }
            snprintf(expect_rel, sizeof expect_rel, "%s/%d-%s", b->version, b->revision, b->release);
            expect(rs_build_archive_relpath(b, rel, sizeof rel) == 0, "relpath fits");
            expect(strcmp(rel, expect_rel) == 0, "relpath matches identity");
            if (b->era == RS_ERA_RSC) {
                expect(rs_pkt_opcode(b, RS_C2S_WALK) == 221, "rsc walk");
                expect(b->sizes_s2c == NULL && b->sizes_c2s == NULL, "rsc has no size table");
                continue;
            }
            for (i = 0; i < (int)(sizeof no_s2c / sizeof no_s2c[0]); i++) {
                if (no_s2c[i] == b->revision) {
                    skip_s2c = 1;
                }
            }
            for (i = 0; i < (int)(sizeof no_c2s / sizeof no_c2s[0]); i++) {
                if (no_c2s[i] == b->revision) {
                    skip_c2s = 1;
                }
            }
            expect(skip_s2c ? b->sizes_s2c == NULL : b->sizes_s2c != NULL, "s2c sizes");
            expect(skip_c2s ? b->sizes_c2s == NULL : b->sizes_c2s != NULL, "c2s sizes");
        }
    }

    {
        rs_session sess2;
        rs_buf src, payload;
        uint8_t bytes[8];
        int opcode;
        uint32_t k[4] = {1, 2, 3, 4};
        rs_isaac client_out;

        rs_session_init(&sess2, rs_build_lookup(317));
        rs_session_seed_isaac(&sess2, k);
        rs_isaac_init(&client_out, k);
        bytes[0] = (uint8_t)((164 + rs_isaac_next(&client_out)) & 0xff);
        rs_buf_wrap(&src, bytes, 1);
        expect_err(rs_session_read_c2s(&sess2, &src, &opcode, &payload), RS_ERR_NEED_MORE,
                   "walk need more");
        bytes[1] = 3;
        rs_buf_wrap(&src, bytes, 2);
        expect_err(rs_session_read_c2s(&sess2, &src, &opcode, &payload), RS_ERR_NEED_MORE,
                   "walk still need more");
        bytes[2] = 1;
        bytes[3] = 2;
        bytes[4] = 3;
        rs_buf_wrap(&src, bytes, 5);
        expect_err(rs_session_read_c2s(&sess2, &src, &opcode, &payload), RS_OK, "walk complete");
        expect(opcode == 164, "walk opcode after rewind");
    }
    {
        rs_session rsc;
        rs_buf dst, src, payload;
        uint8_t msg[3] = {'h', 'i', '!'};
        int opcode;
        rs_session_init(&rsc, rs_build_lookup(177));
        expect_err(rs_buf_alloc(&dst, 32), RS_OK, "rsc buf");
        expect_err(rs_session_write_s2c(&rsc, &dst, 8, msg, 3), RS_OK, "rsc write");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_session_read_c2s(&rsc, &src, &opcode, &payload), RS_OK, "rsc read");
        expect(opcode == 8, "rsc opcode roundtrip");
        rs_buf_free(&dst);
    }

    hsbuf[0] = 14;
    hsbuf[1] = 7;
    expect_err(rs_handshake_parse(rs_build_lookup(317), hsbuf, 2, &hs), RS_OK, "hs login");
    expect(hs.link == RS_LINK_LOGIN && hs.consumed == 2, "hs login fields");
    hsbuf[0] = 15;
    expect_err(rs_handshake_parse(rs_build_lookup(317), hsbuf, 1, &hs), RS_OK, "hs ondemand");
    expect(hs.link == RS_LINK_ONDEMAND, "ondemand link");
    {
        rs_session s317, s474;
        rs_update_req up;
        size_t ncons = 0;
        uint8_t od[4] = {1, 0, 2, 2};
        rs_session_init(&s317, rs_build_lookup(317));
        expect_err(rs_session_handshake_parse(&s317, hsbuf, 1, &hs), RS_OK, "session hs ondemand");
        expect_err(rs_session_update_parse(&s317, od, 4, &up, &ncons), RS_OK, "session update parse");
        expect(up.archive == 1 && up.id == 2 && ncons == 4, "ondemand fields");
        rs_session_init(&s474, rs_build_lookup(474));
        hsbuf[0] = 15;
        hsbuf[1] = 0;
        hsbuf[2] = 0;
        hsbuf[3] = 0x01;
        hsbuf[4] = 0xDA;
        expect_err(rs_session_handshake_parse(&s474, hsbuf, 5, &hs), RS_OK, "session hs js5");
        expect(hs.link == RS_LINK_JS5 && hs.revision == 474, "js5 revision");
    }

    expect(rs_proto_for(rs_build_lookup(177)) == rs_proto_rsc(), "115/177 proto is rsc");
    expect(rs_proto_for(rs_build_lookup(317)) == rs_proto_rs2(), "317 proto is rs2");
    expect(rs_proto_for(rs_build_lookup(468)) == rs_proto_js5(), "468 proto is js5");
    expect(rs_proto_for(rs_build_lookup(474)) == rs_proto_js5(), "474 proto is js5");
    {
        rs_session cli, srv;
        rs_buf dst, src, payload;
        rs_handshake hs2;
        rs_update_req up;
        size_t ncons = 0;
        uint8_t walk[3] = {1, 2, 3};
        uint32_t k[4] = {11, 22, 33, 44};
        int opcode;

        rs_session_init_role(&cli, rs_build_lookup(317), RS_ROLE_CLIENT);
        rs_session_init_role(&srv, rs_build_lookup(317), RS_ROLE_SERVER);
        expect(cli.ops == rs_proto_rs2() && srv.ops == rs_proto_rs2(), "317 ops");
        expect_err(rs_buf_alloc(&dst, 64), RS_OK, "role buf");
        expect_err(rs_session_handshake_write(&cli, &dst, RS_LINK_LOGIN, 7), RS_OK,
                   "client handshake write");
        expect_err(rs_session_handshake_parse(&srv, dst.data, dst.pos, &hs2), RS_OK,
                   "server handshake parse");
        expect(hs2.link == RS_LINK_LOGIN && hs2.name_hash == 7, "handshake roundtrip");

        dst.pos = 0;
        expect_err(rs_session_handshake_write(&cli, &dst, RS_LINK_ONDEMAND, 0), RS_OK,
                   "client ondemand hs");
        expect_err(rs_session_handshake_parse(&srv, dst.data, dst.pos, &hs2), RS_OK,
                   "server ondemand parse");
        expect(hs2.link == RS_LINK_ONDEMAND, "ondemand from client write");

        rs_session_seed_isaac(&cli, k);
        rs_session_seed_isaac(&srv, k);
        dst.pos = 0;
        expect_err(rs_session_write(&cli, &dst, 164, walk, 3), RS_OK, "client write walk");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_session_read(&srv, &src, &opcode, &payload), RS_OK, "server read walk");
        expect(opcode == 164 && payload.cap == 3, "walk opcode+size via shared API");

        {
            rs_session c474, s474;
            rs_session_init_role(&c474, rs_build_lookup(474), RS_ROLE_CLIENT);
            rs_session_init_role(&s474, rs_build_lookup(474), RS_ROLE_SERVER);
            dst.pos = 0;
            expect_err(rs_session_handshake_write(&c474, &dst, RS_LINK_JS5, 0), RS_OK, "js5 hs write");
            expect_err(rs_session_handshake_parse(&s474, dst.data, dst.pos, &hs2), RS_OK, "js5 hs parse");
            expect(hs2.link == RS_LINK_JS5 && hs2.revision == 474, "js5 hs from client write");
            dst.pos = 0;
            up.opcode = 1;
            up.archive = 0;
            up.id = 1;
            up.priority = 2;
            expect_err(rs_session_update_write(&c474, &dst, &up), RS_OK, "js5 update write");
            expect_err(rs_session_update_parse(&s474, dst.data, dst.pos, &up, &ncons), RS_OK,
                       "js5 update parse");
            expect(up.archive == 0 && up.id == 1 && ncons == 4, "js5 474 request is 4 bytes");
            dst.pos = 0;
            memset(&up, 0, sizeof(up));
            up.opcode = RS_JS5_ENCRYPT;
            up.key = 0x5A;
            expect_err(rs_session_update_write(&c474, &dst, &up), RS_OK, "474 encrypt write");
            expect(dst.pos == 4 && dst.data[0] == RS_JS5_ENCRYPT && dst.data[3] == 0x5A,
                   "474 encrypt key is the last byte");
            {
                rs_session s910;
                rs_session_init(&s910, rs_build_lookup(910));
                dst.pos = 0;
                memset(&up, 0, sizeof(up));
                up.opcode = RS_JS5_ENCRYPT;
                up.key = 0x5A;
                expect_err(rs_session_update_write(&s910, &dst, &up), RS_OK, "910 encrypt write");
                expect(dst.pos == 6 && dst.data[1] == 0x5A, "910 encrypt key is byte 1");
                expect_err(rs_session_update_parse(&s910, dst.data, dst.pos, &up, &ncons), RS_OK,
                           "910 encrypt parse");
                expect(ncons == 6 && up.archive == 0x5A && s910.js5_key == 0x5A, "910 encrypt key");
                dst.pos = 0;
                memset(&up, 0, sizeof(up));
                up.opcode = RS_JS5_CONNECTED;
                expect_err(rs_session_update_write(&s910, &dst, &up), RS_OK, "910 connected write");
                expect(dst.pos == 6 && dst.data[0] == RS_JS5_CONNECTED && dst.data[3] == 4,
                       "910 connected is 6 bytes");
            }
        }
        rs_buf_free(&dst);
    }

    {
        rs_buf pkt, src, payload;
        uint8_t storage[64];
        int opcode;
        uint32_t k[4] = {9, 8, 7, 6};
        rs_session_init(&sess, rs_build_lookup(317));
        rs_session_seed_isaac(&sess, k);
        rs_buf_wrap(&pkt, storage, sizeof(storage));
        pkt.owned = 0;
        {
            rs_isaac client_out;
            rs_isaac_init(&client_out, k);
            expect_err(rs_p1(&pkt, (int)((0 + rs_isaac_next(&client_out)) & 0xff)), RS_OK,
                       "idle opcode");
        }
        rs_buf_wrap(&src, storage, pkt.pos);
        expect_err(rs_session_read_c2s(&sess, &src, &opcode, &payload), RS_OK, "read idle");
        expect(opcode == 0, "idle opcode decoded");
    }

#ifdef RSARCHIVE_ROOT
    {
        char path[512];
        rs_jag jag;
        rs_blob file;
        rs_err je;
        snprintf(path, sizeof(path), "%s/builds/rsc/115-2001-12-24/cache/config48.jag", RSARCHIVE_ROOT);
        je = rs_jag_open_file(&jag, path);
        expect_err(je, RS_OK, "open rsc jag");
        if (je == RS_OK) {
            expect(jag.file_count > 0, "jag has files");
            if (jag.file_count > 0) {
                expect_err(rs_jag_read_hash(&jag, jag.files[0].hash, &file), RS_OK, "jag first file");
                expect(file.size > 0, "jag file size");
                rs_blob_free(&file);
            }
            rs_jag_close(&jag);
        }
    }
    {
        char path[512];
        rs_cache *cache;
        rs_blob raw;
        rs_jag jag;
        expect_err(rs_ctx_set_build_revision(ctx, 317), RS_OK, "ctx 317 for cache");
        snprintf(path, sizeof(path), "%s/builds/rs2/317-2005-06-13/cache", RSARCHIVE_ROOT);
        cache = rs_cache_open(ctx, path);
        expect(cache != NULL, "open 317 cache");
        if (cache) {
            rs_err re = rs_cache_read(cache, 0, 1, &raw);
            rs_err oe;
            expect_err(re, RS_OK, "read 317 title archive");
            if (re == RS_OK) {
                expect(raw.size > 1000, "title archive size");
                oe = rs_jag_open_mem(&jag, raw.data, raw.size);
                expect_err(oe, RS_OK, "unpack title jag");
                if (oe == RS_OK) {
                    expect(jag.file_count > 0, "title jag files");
                    rs_jag_close(&jag);
                }
                rs_blob_free(&raw);
            }
            if (rs_cache_read_named(cache, "obj.dat", &raw) == RS_OK) {
                expect(raw.size > 0, "named obj.dat");
                rs_blob_free(&raw);
            }
            re = rs_cache_get(cache, 0, 1, -1, &raw);
            expect_err(re, RS_OK, "cache_get title");
            if (re == RS_OK) {
                expect(raw.size > 1000, "cache_get size");
                rs_blob_free(&raw);
            }
            rs_cache_close(cache);
        }
    }
    {
        char path[512];
        rs_cache *cache;
        rs_js5_index idx;
        expect_err(rs_ctx_set_build_revision(ctx, 468), RS_OK, "ctx 468");
        snprintf(path, sizeof(path), "%s/builds/rs2/468-2007-08-08/cache", RSARCHIVE_ROOT);
        cache = rs_cache_open(ctx, path);
        expect(cache != NULL, "open 468 cache");
        if (cache) {
            expect_err(rs_cache_index(cache, 0, &idx), RS_OK, "parse 468 js5 index 0");
            expect(idx.ngroups > 0, "468 index 0 groups");
            rs_js5_index_free(&idx);
            rs_cache_close(cache);
        }
        expect_err(rs_ctx_set_build_revision(ctx, 474), RS_OK, "ctx 474");
        snprintf(path, sizeof(path), "%s/builds/rs2/474-2007-10-19/cache", RSARCHIVE_ROOT);
        cache = rs_cache_open(ctx, path);
        expect(cache != NULL, "open 474 cache");
        if (cache) {
            expect_err(rs_cache_index(cache, 0, &idx), RS_OK, "parse js5 index 0");
            expect(idx.ngroups > 0, "index 0 groups");
            expect(idx.format >= 5, "index format");
            rs_js5_index_free(&idx);
            rs_cache_close(cache);
        }
    }
    {
        char path[512];
        rs_cache *cache;
        expect_err(rs_ctx_set_build_id(ctx, "2001-115"), RS_OK, "ctx rsc");
        snprintf(path, sizeof(path), "%s/builds/rsc/115-2001-12-24/cache", RSARCHIVE_ROOT);
        cache = rs_cache_open(ctx, path);
        expect(cache != NULL, "open rsc cache dir");
        if (cache) {
            expect(rs_cache_archive_count(cache) >= 5, "rsc jag count");
            rs_cache_close(cache);
        }
    }
#endif

    {
        rs_buf chat;
        char out[64];
        expect_err(rs_buf_alloc(&chat, 32), RS_OK, "chat buf");
        expect_err(rs_wordpack_encode("hello.", &chat), RS_OK, "wordpack encode");
        chat.pos = 0;
        expect_err(rs_wordpack_decode(&chat, (int)chat.cap, out, sizeof(out)), RS_OK,
                   "wordpack decode");
        expect(out[0] == 'H' || out[0] == 'h', "wordpack starts");
        rs_buf_free(&chat);
    }
    {
        rs_rsa_key pub, priv;
        uint8_t enc[64], dec[64];
        size_t n = 0, m = 0;
        const uint8_t msg[1] = {7};
        expect_err(rs_rsa_set_decimal(&pub, "3233", "17"), RS_OK, "tiny rsa public");
        expect_err(rs_rsa_set_private_decimal(&priv, "3233", "2753"), RS_OK, "tiny rsa private");
        expect_err(rs_rsa_modpow(&pub, msg, 1, enc, &n), RS_OK, "rsa encrypt");
        expect_err(rs_rsa_modpow_key(&priv, 1, enc, n, dec, &m), RS_OK, "rsa decrypt");
        expect(m >= 1 && dec[m - 1] == 7, "rsa roundtrip");
    }
    {
        rs_session s;
        rs_login_request req, got;
        rs_buf dst, src;
        memset(&req, 0, sizeof(req));
        req.revision = 317;
        req.uid = 7;
        strcpy(req.username, "player1");
        strcpy(req.password, "pass");
        req.isaac_seed[0] = 11;
        req.isaac_seed[1] = 22;
        req.isaac_seed[2] = 33;
        req.isaac_seed[3] = 44;
        rs_session_init(&s, rs_build_lookup(317));
        expect_err(rs_buf_alloc(&dst, 256), RS_OK, "login buf");
        expect_err(rs_session_login_write(&s, &dst, &req, NULL), RS_OK, "317 login write");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_session_login_parse(&s, &src, NULL, &got), RS_OK, "317 login parse");
        expect(got.uid == 7 && strcmp(got.username, "player1") == 0, "317 login fields");
        expect(got.isaac_seed[0] == 11 && got.ncrc == 9, "317 crc+isaac");
        rs_buf_free(&dst);
    }
    {
        rs_session s;
        rs_login_request req, got;
        rs_buf dst, src;
        memset(&req, 0, sizeof(req));
        strcpy(req.username, "js5user");
        strcpy(req.password, "secret");
        req.isaac_seed[0] = 1;
        rs_session_init(&s, rs_build_lookup(474));
        expect_err(rs_buf_alloc(&dst, 512), RS_OK, "474 login buf");
        expect_err(rs_session_login_write(&s, &dst, &req, NULL), RS_OK, "474 login write");
        expect(dst.data[0] == 16 && dst.data[1] == 0, "474 size is u16");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_session_login_parse(&s, &src, NULL, &got), RS_OK, "474 login parse");
        expect(strcmp(got.username, "js5user") == 0, "474 username");
        rs_buf_free(&dst);
    }
    {
        rs_session s;
        rs_login_request req, got;
        rs_buf dst, src;
        memset(&req, 0, sizeof(req));
        req.lobby = 1;
        req.revision = 910;
        strcpy(req.username, "lobby");
        strcpy(req.password, "secret");
        req.isaac_seed[0] = 1;
        req.isaac_seed[1] = 2;
        req.isaac_seed[2] = 3;
        req.isaac_seed[3] = 4;
        req.window_mode = 1;
        req.canvas_w = 800;
        req.canvas_h = 600;
        req.archive_crc[0] = 0x11111111u;
        req.archive_crc[40] = 0x22222222u;
        rs_session_init(&s, rs_build_lookup(910));
        expect(s.ops == rs_proto_js5_lobby(), "910 session uses lobby");
        expect_err(rs_buf_alloc(&dst, 1024), RS_OK, "910 lobby buf");
        expect_err(rs_session_login_write(&s, &dst, &req, NULL), RS_OK, "910 lobby write");
        expect(dst.data[0] == 19, "910 lobby opcode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_session_login_parse(&s, &src, NULL, &got), RS_OK, "910 lobby parse");
        expect(got.lobby == 1 && strcmp(got.username, "lobby") == 0 &&
                   strcmp(got.password, "secret") == 0,
               "910 lobby identity");
        expect(got.isaac_seed[0] == 1 && got.isaac_seed[3] == 4, "910 lobby seeds");
        expect(got.ncrc == 41 && got.archive_crc[0] == 0x11111111u &&
                   got.archive_crc[40] == 0x22222222u,
               "910 lobby crcs");
        expect(got.window_mode == 1 && got.canvas_w == 800 && got.canvas_h == 600,
               "910 lobby window");
        dst.pos = 0;
        req.lobby = 0;
        expect_err(rs_session_login_write(&s, &dst, &req, NULL), RS_OK, "910 game login write");
        expect(dst.data[0] == 16, "910 game login opcode");
        rs_buf_free(&dst);
    }
    {
        rs_session s;
        rs_buf dst, src;
        rs_walk w, w2;
        rs_player_update pu, pu2;
        rs_session_init(&s, rs_build_lookup(317));
        memset(&w, 0, sizeof(w));
        w.dest_x = 3200;
        w.dest_z = 3200;
        w.running = 1;
        expect_err(rs_buf_alloc(&dst, 64), RS_OK, "walk buf");
        expect_err(rs_codec_walk_encode(&s, &dst, &w), RS_OK, "walk encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_walk_decode(&s, &src, &w2), RS_OK, "walk decode");
        expect(w2.dest_x == 3200 && w2.dest_z == 3200 && w2.running, "walk fields");
        dst.pos = 0;
        memset(&pu, 0, sizeof(pu));
        pu.teleport = 1;
        pu.plane = 0;
        pu.local_x = 10;
        pu.local_z = 20;
        pu.include_appearance = 1;
        pu.appearance.gender = 0;
        expect_err(rs_codec_player_update_encode(&s, &dst, &pu), RS_OK, "player update encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_player_update_decode(&s, &src, &pu2), RS_OK, "player update decode");
        expect(pu2.teleport && pu2.local_x == 10 && pu2.local_z == 20, "player update fields");
        rs_buf_free(&dst);
    }
    {
        rs_worldlist wl, wl2;
        rs_buf dst, src;
        memset(&wl, 0, sizeof(wl));
        wl.ncountries = 1;
        strcpy(wl.countries[0].name, "UK");
        wl.min_id = 1;
        wl.max_id = 1;
        wl.nworlds = 1;
        wl.worlds[0].id = 1;
        strcpy(wl.worlds[0].hostname, "127.0.0.1");
        strcpy(wl.worlds[0].activity, "test");
        expect_err(rs_buf_alloc(&dst, 256), RS_OK, "wl buf");
        expect_err(rs_worldlist_encode(&dst, &wl), RS_OK, "wl encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_worldlist_decode(&src, &wl2), RS_OK, "wl decode");
        expect(wl2.nworlds == 1 && strcmp(wl2.worlds[0].hostname, "127.0.0.1") == 0, "wl fields");
        rs_buf_free(&dst);
    }
    expect(rs_pkt_opcode(rs_build_lookup(742), RS_C2S_WINDOW_STATUS) == 31, "742 window");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_S2C_LOC_ADD) == 151, "317 loc_add");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_S2C_OBJ_ADD) == 44, "317 obj_add");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_S2C_ZONE_BASE) == 85, "317 zone_base");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_NPC5) == 153, "317 npc5");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_OPHELD) == 122, "317 opheld");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_IF_OPTION1) == 145, "317 if1");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_IF_OPTION2) == 117, "317 if2");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_IF_OPTION3) == 43, "317 if3");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_IF_OPTION4) == 129, "317 if4");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_IF_OPTION5) == 135, "317 if5");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_S2C_AMOUNT_PROMPT) == 27, "317 amount prompt");
    expect(RS_C2S_IF_OPTION5 == RS_C2S_IF_OPTION1 + 4, "317 if options contiguous");
    expect(rs_sizes_c2s_317[145] == 6 && rs_sizes_c2s_317[117] == 6 && rs_sizes_c2s_317[43] == 6 &&
               rs_sizes_c2s_317[129] == 6 && rs_sizes_c2s_317[135] == 6 && rs_sizes_c2s_317[122] == 6 &&
               rs_sizes_c2s_317[208] == 4 && rs_sizes_s2c_317[27] == 0,
           "317 bank sizes");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_C2S_MAGIC_ON_GROUND) == 181, "317 magic_ground");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_S2C_IF_SETSCROLL) == 79, "317 if_setscroll");
    expect(rs_pkt_opcode(rs_build_lookup(317), RS_S2C_IF_OPENTOP) == 248, "317 if_opentop");
    expect(rs_pkt_opcode(rs_build_lookup(474), RS_S2C_REGION) == 61, "474 region");
    expect(rs_pkt_opcode(rs_build_lookup(474), RS_S2C_PLAYER_UPDATE) == 150, "474 player");
    expect(rs_pkt_opcode(rs_build_lookup(474), RS_S2C_MESSAGE) == 209, "474 message");
    expect(rs_pkt_opcode(rs_build_lookup(474), RS_C2S_CHAT) == 52, "474 chat");
    expect(rs_pkt_opcode(rs_build_lookup(508), RS_S2C_REGION) == 142, "508 region");
    expect(rs_pkt_opcode(rs_build_lookup(508), RS_S2C_PLAYER_UPDATE) == 216, "508 player");
    expect(rs_pkt_opcode(rs_build_lookup(508), RS_S2C_NPC_UPDATE) == 222, "508 npc");
    expect(rs_pkt_opcode(rs_build_lookup(508), RS_S2C_MESSAGE) == 132, "508 message");
    expect(rs_pkt_opcode(rs_build_lookup(508), RS_S2C_LOGOUT) == 104, "508 logout");
    expect(rs_pkt_opcode(rs_build_lookup(508), RS_S2C_INVENTORY) == 135, "508 inventory");
    expect(rs_pkt_opcode(rs_build_lookup(508), RS_S2C_SKILL) == 217, "508 skill");
    {
        rs_session s614, s578;
        rs_buf bad;
        uint8_t one = 1;
        rs_session_init(&s614, rs_build_lookup(614));
        expect(rs_session_size_c2s(&s614, 200) == -3, "614 unused opcode invalid");
        rs_session_init(&s578, rs_build_lookup(578));
        expect_err(rs_buf_alloc(&bad, 16), RS_OK, "578 frame buf");
        expect_err(rs_session_write(&s578, &bad, 1, &one, 1), RS_ERR_PROTO, "578 write fails closed");
        rs_buf_free(&bad);
    }
    {
        rs_session s317, s474, s508, s530, s742, s910;
        rs_buf dst, src;
        rs_region in, out;
        rs_session_init(&s317, rs_build_lookup(317));
        rs_session_init(&s474, rs_build_lookup(474));
        rs_session_init(&s508, rs_build_lookup(508));
        rs_session_init(&s530, rs_build_lookup(530));
        rs_session_init(&s742, rs_build_lookup(742));
        rs_session_init(&s910, rs_build_lookup(910));
        memset(&in, 0, sizeof(in));
        in.zone_x = 402;
        in.zone_z = 402;
        expect_err(rs_buf_alloc(&dst, 512), RS_OK, "317 region buf");
        expect_err(rs_codec_region_encode(&s317, &dst, &in), RS_OK, "317 region encode");
        expect(dst.pos == 4, "317 region is two u16");
        expect((dst.data[1] & 0xff) == ((402 + 128) & 0xff), "317 region zone_x is g2_alt2");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_region_decode(&s317, &src, &out), RS_OK, "317 region decode");
        expect(out.zone_x == 402 && out.zone_z == 402, "317 region fields");
        dst.pos = 0;
        in.plane = 1;
        in.local_x = 54;
        in.local_z = 54;
        expect_err(rs_codec_region_encode(&s474, &dst, &in), RS_OK, "474 region encode");
        expect(dst.pos > 4, "474 rebuild is larger than two u16");
        expect(dst.pos == 153, "474 lumbridge rebuild is 9 mapsquares");
        rs_buf_wrap(&src, dst.data, dst.pos);
        memset(&out, 0, sizeof(out));
        expect_err(rs_codec_region_decode(&s474, &src, &out), RS_OK, "474 region decode");
        expect(out.zone_x == 402 && out.zone_z == 402, "474 zone");
        expect(out.plane == 1 && out.local_x == 54 && out.local_z == 54, "474 local/plane");
        dst.pos = 0;
        expect_err(rs_codec_region_encode(&s508, &dst, &in), RS_OK, "508 region encode");
        expect(dst.pos == 153, "508 lumbridge rebuild is 9 mapsquares");
        rs_buf_wrap(&src, dst.data, dst.pos);
        memset(&out, 0, sizeof(out));
        expect_err(rs_codec_region_decode(&s508, &src, &out), RS_OK, "508 region decode");
        expect(out.zone_x == 402 && out.zone_z == 402, "508 zone");
        expect(out.plane == 1 && out.local_x == 54 && out.local_z == 54, "508 local/plane");
        dst.pos = 0;
        in.npc_view = 14;
        in.build_area = 1;
        in.force_reload = 1;
        expect_err(rs_codec_region_encode(&s530, &dst, &in), RS_OK, "530 region encode");
        expect(dst.pos == 153, "530 lumbridge rebuild is 9 mapsquares");
        rs_buf_wrap(&src, dst.data, dst.pos);
        memset(&out, 0, sizeof(out));
        expect_err(rs_codec_region_decode(&s530, &src, &out), RS_OK, "530 region decode");
        expect(out.zone_x == 402 && out.zone_z == 402, "530 zone");
        expect(out.plane == 1 && out.local_x == 54 && out.local_z == 54, "530 local/plane");
        dst.pos = 0;
        expect_err(rs_codec_region_encode(&s910, &dst, &in), RS_OK, "910 region encode");
        expect(dst.pos == 8, "910 rebuild has no xtea");
        rs_buf_wrap(&src, dst.data, dst.pos);
        memset(&out, 0, sizeof(out));
        expect_err(rs_codec_region_decode(&s910, &src, &out), RS_OK, "910 region decode");
        expect(out.zone_x == 402 && out.zone_z == 402, "910 zone");
        expect(out.npc_view == 14 && out.build_area == 1 && out.force_reload == 1, "910 rebuild flags");
        dst.pos = 0;
        expect_err(rs_codec_region_encode(&s742, &dst, &in), RS_OK, "742 region encode");
        expect(dst.pos > 4, "742 rebuild is larger than two u16");
        dst.pos = 0;
        expect_err(rs_codec_region_encode(NULL, &dst, &in), RS_ERR_UNSUPPORTED, "null session region");
        expect(rs_codec_for(NULL) == &rs_codec_rs2, "null session codec is rs2");
        {
            rs_zone_base zb, zb2;
            rs_scene_loc loc, loc2;
            rs_scene_obj obj, obj2;
            dst.pos = 0;
            memset(&zb, 0, sizeof(zb));
            zb.x = 48;
            zb.z = 40;
            expect_err(rs_codec_zone_base_encode(&s317, &dst, &zb), RS_OK, "zone base encode");
            rs_buf_wrap(&src, dst.data, dst.pos);
            expect_err(rs_codec_zone_base_decode(&s317, &src, &zb2), RS_OK, "zone base decode");
            expect(zb2.x == 48 && zb2.z == 40, "zone base fields");
            dst.pos = 0;
            memset(&loc, 0, sizeof(loc));
            loc.x = 3;
            loc.z = 5;
            loc.id = 1276;
            loc.shape = 10;
            loc.rot = 2;
            expect_err(rs_codec_loc_add_encode(&s317, &dst, &loc), RS_OK, "loc add encode");
            rs_buf_wrap(&src, dst.data, dst.pos);
            expect_err(rs_codec_loc_add_decode(&s317, &src, &loc2), RS_OK, "loc add decode");
            expect(loc2.x == 3 && loc2.z == 5 && loc2.id == 1276 && loc2.shape == 10 && loc2.rot == 2,
                   "loc add fields");
            dst.pos = 0;
            expect_err(rs_codec_loc_del_encode(&s317, &dst, &loc), RS_OK, "loc del encode");
            rs_buf_wrap(&src, dst.data, dst.pos);
            expect_err(rs_codec_loc_del_decode(&s317, &src, &loc2), RS_OK, "loc del decode");
            expect(loc2.x == 3 && loc2.z == 5 && loc2.shape == 10 && loc2.rot == 2, "loc del fields");
            dst.pos = 0;
            memset(&obj, 0, sizeof(obj));
            obj.x = 1;
            obj.z = 2;
            obj.id = 995;
            obj.amount = 25;
            expect_err(rs_codec_obj_add_encode(&s317, &dst, &obj), RS_OK, "obj add encode");
            rs_buf_wrap(&src, dst.data, dst.pos);
            expect_err(rs_codec_obj_add_decode(&s317, &src, &obj2), RS_OK, "obj add decode");
            expect(obj2.x == 1 && obj2.z == 2 && obj2.id == 995 && obj2.amount == 25, "obj add fields");
            dst.pos = 0;
            expect_err(rs_codec_obj_del_encode(&s317, &dst, &obj), RS_OK, "obj del encode");
            rs_buf_wrap(&src, dst.data, dst.pos);
            expect_err(rs_codec_obj_del_decode(&s317, &src, &obj2), RS_OK, "obj del decode");
            expect(obj2.x == 1 && obj2.z == 2 && obj2.id == 995, "obj del fields");
        }
        rs_buf_free(&dst);
    }
    {
        rs_ctx *kctx = rs_ctx_create();
        rs_cache *cache;
        const rs_xtea_key *key;
        expect_err(rs_ctx_set_build_id(kctx, "474"), RS_OK, "474 cache ctx");
        cache = rs_cache_open_archive(kctx, RSARCHIVE_ROOT "/builds");
        expect(cache != NULL, "open 474 cache");
        expect(rs_cache_find_key_mapsquare(NULL, 10295) == NULL, "mapsquare null cache");
        if (cache) {
            key = rs_cache_find_key_mapsquare(cache, 10295);
            expect(key != NULL && key->mapsquare == 10295, "mapsquare 40_55");
            expect(key && key->key[0] == (uint32_t)-1920480496, "mapsquare 40_55 key0");
            {
                rs_session s474;
                rs_buf dst, src;
                rs_region in, out;
                rs_session_init(&s474, rs_build_lookup(474));
                memset(&in, 0, sizeof(in));
                in.zone_x = 402;
                in.zone_z = 402;
                in.local_x = 54;
                in.local_z = 54;
                in.cache = cache;
                expect_err(rs_buf_alloc(&dst, 512), RS_OK, "474 keyed region buf");
                expect_err(rs_codec_region_encode(&s474, &dst, &in), RS_OK, "474 keyed encode");
                rs_buf_wrap(&src, dst.data, dst.pos);
                expect_err(rs_codec_region_decode(&s474, &src, &out), RS_OK, "474 keyed decode");
                expect(out.zone_x == 402 && out.local_x == 54, "474 keyed fields");
                rs_buf_free(&dst);
            }
            rs_cache_close(cache);
        }
        rs_ctx_destroy(kctx);
    }
    {
        const char *dir = "/tmp/rslib-whirl-test";
        char path[128];
        uint8_t group[7] = {0, 0, 0, 0, 2, 'h', 'i'};
        uint8_t digest[64];
        uint8_t plain[96];
        uint8_t container[128];
        uint8_t dat[520 * 3];
        uint8_t idxent[6];
        int plen = 0;
        FILE *f;
        rs_ctx *wctx;
        rs_cache *cache;
        rs_blob out;
        rs_whirlpool(group, sizeof(group), digest);
        plain[plen++] = 5;
        plain[plen++] = 2;
        put_be16(plain + plen, 1);
        plen += 2;
        put_be16(plain + plen, 0);
        plen += 2;
        put_be32(plain + plen, 0);
        plen += 4;
        memcpy(plain + plen, digest, 64);
        plen += 64;
        put_be32(plain + plen, 1);
        plen += 4;
        put_be16(plain + plen, 1);
        plen += 2;
        put_be16(plain + plen, 0);
        plen += 2;
        container[0] = 0;
        put_be32(container + 1, (uint32_t)plen);
        memcpy(container + 5, plain, (size_t)plen);
        memset(dat, 0, sizeof(dat));
        put_be16(dat + 520, 0);
        put_be16(dat + 522, 0);
        put_be24(dat + 524, 0);
        dat[527] = 0;
        memcpy(dat + 528, group, sizeof(group));
        put_be16(dat + 1040, 0);
        put_be16(dat + 1042, 0);
        put_be24(dat + 1044, 0);
        dat[1047] = 255;
        memcpy(dat + 1048, container, (size_t)plen + 5);
        mkdir(dir, 0700);
        snprintf(path, sizeof(path), "%s/main_file_cache.dat2", dir);
        f = fopen(path, "wb");
        expect(f != NULL, "whirlpool dat2");
        if (f) {
            expect(fwrite(dat, 1, sizeof(dat), f) == sizeof(dat), "write dat2");
            fclose(f);
        }
        put_be24(idxent, (int)sizeof(group));
        put_be24(idxent + 3, 1);
        snprintf(path, sizeof(path), "%s/main_file_cache.idx0", dir);
        f = fopen(path, "wb");
        if (f) {
            fwrite(idxent, 1, 6, f);
            fclose(f);
        }
        put_be24(idxent, plen + 5);
        put_be24(idxent + 3, 2);
        snprintf(path, sizeof(path), "%s/main_file_cache.idx255", dir);
        f = fopen(path, "wb");
        if (f) {
            fwrite(idxent, 1, 6, f);
            fclose(f);
        }
        wctx = rs_ctx_create();
        expect_err(rs_ctx_set_build_revision(wctx, 443), RS_OK, "whirlpool build");
        cache = rs_cache_open(wctx, dir);
        expect(cache != NULL, "open whirlpool cache");
        if (cache) {
            expect_err(rs_cache_read_group(cache, 0, 0, &out), RS_OK, "whirlpool match");
            expect(out.size == 2 && out.data && out.data[0] == 'h' && out.data[1] == 'i',
                   "whirlpool payload");
            rs_blob_free(&out);
            rs_cache_close(cache);
        }
        dat[1048 + 5 + 10] ^= 0xff;
        snprintf(path, sizeof(path), "%s/main_file_cache.dat2", dir);
        f = fopen(path, "wb");
        if (f) {
            fwrite(dat, 1, sizeof(dat), f);
            fclose(f);
        }
        cache = rs_cache_open(wctx, dir);
        if (cache) {
            expect_err(rs_cache_read_group(cache, 0, 0, &out), RS_ERR_FORMAT, "whirlpool mismatch");
            rs_cache_close(cache);
        }
        rs_ctx_destroy(wctx);
    }
    {
        rs_session s317, s474;
        rs_buf dst, src, other;
        rs_skill skill, skill2;
        rs_varp varp, varp2;
        rs_if_text ift, ift2;
        rs_inventory inv, inv2;
        rs_appearance app, app2;
        rs_spotanim spot, spot2;
        rs_projectile proj, proj2;
        rs_sound snd, snd2;
        rs_player_index pi, pi2;
        rs_player_option opt, opt2;
        rs_friend_status fr, fr2;
        rs_energy en, en2;
        rs_weight wt, wt2;
        rs_session_init(&s317, rs_build_lookup(317));
        rs_session_init(&s474, rs_build_lookup(474));
        expect_err(rs_buf_alloc(&dst, 512), RS_OK, "payload buf");
        expect_err(rs_buf_alloc(&other, 64), RS_OK, "payload other");

        memset(&skill, 0, sizeof(skill));
        skill.id = 5;
        skill.xp = 0x12345678;
        skill.level = 9;
        expect_err(rs_codec_skill_encode(&s317, &dst, &skill), RS_OK, "skill encode");
        expect(dst.pos == 6 && dst.data[0] == 5 && dst.data[1] == 0x56 && dst.data[2] == 0x78 &&
                   dst.data[3] == 0x12 && dst.data[4] == 0x34 && dst.data[5] == 9,
               "skill xp is g4_alt2");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_skill_decode(&s317, &src, &skill2), RS_OK, "skill decode");
        expect(skill2.id == 5 && skill2.xp == 0x12345678 && skill2.level == 9, "skill fields");
        other.pos = 0;
        expect_err(rs_codec_skill_encode(&s474, &other, &skill), RS_OK, "474 skill encode");
        expect(other.pos == 6 && other.data[0] == 5 && other.data[1] == 0x34 && other.data[2] == 0x12 &&
                   other.data[3] == 0x78 && other.data[4] == 0x56 && other.data[5] == 9,
               "474 skill is g1, g4_alt3, g1");
        expect_err(rs_codec_skill_encode(NULL, &other, &skill), RS_ERR_ARG, "null skill encode");

        dst.pos = 0;
        memset(&varp, 0, sizeof(varp));
        varp.id = 300;
        varp.value = 5;
        expect_err(rs_codec_varp_encode(&s317, &dst, &varp), RS_OK, "varp small encode");
        expect(dst.pos == 3 && dst.data[0] == 0x2c && dst.data[1] == 0x01 && dst.data[2] == 0x05,
               "varp small is g2_alt1 then g1b");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_varp_decode(&s317, &src, &varp2), RS_OK, "varp small decode");
        expect(varp2.id == 300 && varp2.value == 5, "varp small fields");
        dst.pos = 0;
        varp.value = 0x12345678;
        expect_err(rs_codec_varp_encode(&s317, &dst, &varp), RS_OK, "varp large encode");
        expect(dst.pos == 6 && dst.data[0] == 0x2c && dst.data[1] == 0x01 && dst.data[2] == 0x56 &&
                   dst.data[3] == 0x78 && dst.data[4] == 0x12 && dst.data[5] == 0x34,
               "varp large is g2_alt1 then g4_alt2");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_varp_decode(&s317, &src, &varp2), RS_OK, "varp large decode");
        expect(varp2.id == 300 && varp2.value == 0x12345678, "varp large fields");

        dst.pos = 0;
        memset(&ift, 0, sizeof(ift));
        strcpy(ift.text, "Hi");
        ift.component = 100;
        expect_err(rs_codec_if_text_encode(&s317, &dst, &ift), RS_OK, "if text encode");
        expect(dst.pos == 5 && dst.data[0] == 'H' && dst.data[1] == 'i' && dst.data[2] == '\n' &&
                   dst.data[3] == 0 && dst.data[4] == 228,
               "if text is string then g2_alt2");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_if_text_decode(&s317, &src, &ift2), RS_OK, "if text decode");
        expect(strcmp(ift2.text, "Hi") == 0 && ift2.component == 100, "if text fields");

        dst.pos = 0;
        memset(&inv, 0, sizeof(inv));
        inv.component = 100;
        inv.nslots = 2;
        inv.slots[0].id = 50;
        inv.slots[0].amount = 300;
        inv.slots[1].id = 0;
        inv.slots[1].amount = 1;
        expect_err(rs_codec_inventory_encode(&s317, &dst, &inv), RS_OK, "inventory encode");
        expect(dst.data[4] == 255 && dst.data[5] == 0 && dst.data[6] == 0 && dst.data[7] == 0x2c &&
                   dst.data[8] == 1 && dst.data[9] == 178 && dst.data[10] == 0,
               "inventory amount then g2_alt3 id");
        expect(dst.data[11] == 1 && dst.data[12] == 128 && dst.data[13] == 0, "inventory empty id 0");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_inventory_decode(&s317, &src, &inv2), RS_OK, "inventory decode");
        expect(inv2.component == 100 && inv2.nslots == 2 && inv2.slots[0].id == 50 &&
                   inv2.slots[0].amount == 300 && inv2.slots[1].id == 0 && inv2.slots[1].amount == 1,
               "inventory fields");

        dst.pos = 0;
        memset(&app, 0, sizeof(app));
        strcpy(app.name, "zeb");
        app.combat_level = 3;
        app.skill_level = 100;
        app.styles[1] = 10;
        expect_err(rs_codec_appearance_encode(&s317, &dst, &app), RS_OK, "appearance encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_appearance_decode(&s317, &src, &app2), RS_OK, "appearance decode");
        expect(strcmp(app2.name, "zeb") == 0 && app2.combat_level == 3 && app2.skill_level == 100 &&
                   app2.styles[1] == 10 && app2.anims[0] == 0x328,
               "appearance name combat skill style anim");

        dst.pos = 0;
        memset(&spot, 0, sizeof(spot));
        spot.x = 3;
        spot.z = 5;
        spot.id = 100;
        spot.height = 20;
        spot.delay = 5;
        expect_err(rs_codec_spotanim_encode(&s317, &dst, &spot), RS_OK, "spotanim encode");
        expect(dst.pos == 6 && dst.data[0] == 53, "spotanim offset");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_spotanim_decode(&s317, &src, &spot2), RS_OK, "spotanim decode");
        expect(spot2.x == 3 && spot2.z == 5 && spot2.id == 100 && spot2.height == 20 &&
                   spot2.delay == 5,
               "spotanim fields");

        dst.pos = 0;
        memset(&proj, 0, sizeof(proj));
        proj.x = 1;
        proj.z = 2;
        proj.dx = -5;
        proj.dz = -9;
        proj.target = -100;
        proj.graphic = 90;
        proj.start_height = 10;
        proj.end_height = 4;
        proj.start_delay = 20;
        proj.end_delay = 40;
        proj.angle = 16;
        proj.steepness = 8;
        expect_err(rs_codec_projectile_encode(&s317, &dst, &proj), RS_OK, "projectile encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_projectile_decode(&s317, &src, &proj2), RS_OK, "projectile decode");
        expect(proj2.dx == -5 && proj2.dz == -9 && proj2.target == -100 && proj2.graphic == 90 &&
                   proj2.x == 1 && proj2.z == 2,
               "projectile signed fields");

        dst.pos = 0;
        memset(&snd, 0, sizeof(snd));
        snd.id = 12;
        snd.loops = 2;
        snd.delay = 40;
        expect_err(rs_codec_sound_encode(&s317, &dst, &snd), RS_OK, "sound encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_sound_decode(&s317, &src, &snd2), RS_OK, "sound decode");
        expect(snd2.id == 12 && snd2.loops == 2 && snd2.delay == 40, "sound fields");

        dst.pos = 0;
        memset(&pi, 0, sizeof(pi));
        pi.members = 1;
        pi.index = 1;
        expect_err(rs_codec_player_index_encode(&s317, &dst, &pi), RS_OK, "player index encode");
        expect(dst.pos == 3 && dst.data[0] == 129 && dst.data[1] == 129 && dst.data[2] == 0,
               "player index g1_alt1 g2_alt3");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_player_index_decode(&s317, &src, &pi2), RS_OK, "player index decode");
        expect(pi2.members == 1 && pi2.index == 1, "player index fields");

        dst.pos = 0;
        memset(&opt, 0, sizeof(opt));
        opt.slot = 3;
        opt.push_down = 0;
        strcpy(opt.text, "Trade");
        expect_err(rs_codec_player_option_encode(&s317, &dst, &opt), RS_OK, "player option encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_player_option_decode(&s317, &src, &opt2), RS_OK, "player option decode");
        expect(opt2.slot == 3 && opt2.push_down == 0 && strcmp(opt2.text, "Trade") == 0,
               "player option fields");

        dst.pos = 0;
        fr.status = 2;
        expect_err(rs_codec_friend_status_encode(&s317, &dst, &fr), RS_OK, "friend status encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_friend_status_decode(&s317, &src, &fr2), RS_OK, "friend status decode");
        expect(fr2.status == 2, "friend status field");

        dst.pos = 0;
        en.energy = 75;
        expect_err(rs_codec_energy_encode(&s317, &dst, &en), RS_OK, "energy encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_energy_decode(&s317, &src, &en2), RS_OK, "energy decode");
        expect(en2.energy == 75, "energy field");

        dst.pos = 0;
        wt.weight = -40;
        expect_err(rs_codec_weight_encode(&s317, &dst, &wt), RS_OK, "weight encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_weight_decode(&s317, &src, &wt2), RS_OK, "weight decode");
        expect(wt2.weight == -40, "weight signed");

        expect(rs_codec_js5_474.skill_encode == rs_codec_rs2.skill_encode &&
                   rs_codec_js5_474.skill_encode != NULL,
               "474 skill delegate is 317");
        expect(rs_codec_js5_474.walk_encode != rs_codec_rs2.walk_encode, "474 walk delegate differs");
        expect(rs_codec_rs3.projectile_encode != NULL, "rs3 projectile delegate");
        expect(rs_codec_plain.weight_encode == rs_codec_rs2.weight_encode, "plain weight is 317");
        expect(rs_codec_js5_508.weight_encode == rs_codec_rs2.weight_encode, "508 weight is 317");
        expect(rs_codec_js5_530.skill_encode == rs_codec_rs2.skill_encode, "530 skill is 317");

        rs_buf_free(&dst);
        rs_buf_free(&other);
    }
    {
        rs_session s317, s474, s177;
        rs_buf dst, src;
        rs_chat_in chat, chat2;
        rs_if_pos ifp, ifp2;
        rs_sidebar side, side2;
        rs_song song, song2;
        rs_friend_entry fr, fr2;
        rs_zone_full zone, zone2;
        rs_loc_anim loc, loc2;
        rs_hint hint, hint2;
        rs_inv_update invu, invu2;
        rs_command cmd, cmd2;
        rs_region_instanced map, map2;
        rs_last_login login, login2;
        int p, z, x, nl;
        size_t i;

        rs_session_init(&s317, rs_build_lookup(317));
        rs_session_init(&s474, rs_build_lookup(474));
        rs_session_init(&s177, rs_build_lookup(177));
        expect_err(rs_buf_alloc(&dst, 512), RS_OK, "more codec buf");

        dst.pos = 0;
        memset(&chat, 0, sizeof(chat));
        chat.colour = 1;
        chat.effect = 2;
        strcpy(chat.text, "hi");
        expect_err(rs_codec_chat_in_encode(&s317, &dst, &chat), RS_OK, "chat in encode");
        expect(dst.pos >= 2 && dst.data[0] == 127 && dst.data[1] == 126, "chat in colour effect header");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_chat_in_decode(&s317, &src, &chat2), RS_OK, "chat in decode");
        /* Wordpack capitalizes the first letter. */
        expect((strcmp(chat2.text, "hi") == 0 || strcmp(chat2.text, "Hi") == 0) &&
                   chat2.colour == 1 && chat2.effect == 2,
               "chat in round-trips text hi");

        dst.pos = 0;
        expect_err(rs_codec_chat_in_encode(&s474, &dst, &chat), RS_OK, "474 chat in encode");
        expect(!(dst.pos >= 2 && dst.data[0] == 127 && dst.data[1] == 126),
               "474 chat does not start 127,126");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_chat_in_decode(&s474, &src, &chat2), RS_OK, "474 chat in decode");
        expect((strcmp(chat2.text, "hi") == 0 || strcmp(chat2.text, "Hi") == 0) &&
                   chat2.colour == 0 && chat2.effect == 0,
               "474 chat is wordpack only");
        expect(rs_codec_js5_474.chat_in_encode != rs_codec_rs2.chat_in_encode,
               "474 chat in delegate differs");
        expect(rs_codec_js5_474.song_encode == rs_codec_rs2.song_encode &&
                   rs_codec_js5_474.song_encode != NULL,
               "474 song delegate is 317");
        expect(rs_codec_rs3.chat_in_encode == rs_codec_js5_474.chat_in_encode,
               "rs3 chat in delegate is 474");
        expect_err(rs_codec_chat_in_encode(&s177, &dst, &chat), RS_ERR_UNSUPPORTED, "rsc chat in");

        dst.pos = 0;
        memset(&ifp, 0, sizeof(ifp));
        ifp.component = 50;
        ifp.x = -3;
        ifp.y = 4;
        expect_err(rs_codec_if_pos_encode(&s317, &dst, &ifp), RS_OK, "if pos encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_if_pos_decode(&s317, &src, &ifp2), RS_OK, "if pos decode");
        expect(ifp2.component == 50 && ifp2.x == -3 && ifp2.y == 4, "if pos fields");

        dst.pos = 0;
        memset(&side, 0, sizeof(side));
        side.interface_id = -1;
        side.tab = 3;
        expect_err(rs_codec_sidebar_encode(&s317, &dst, &side), RS_OK, "sidebar encode");
        expect(dst.pos == 3 && dst.data[0] == 0xff && dst.data[1] == 0xff && dst.data[2] == 131,
               "sidebar bytes");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_sidebar_decode(&s317, &src, &side2), RS_OK, "sidebar decode");
        expect(side2.interface_id == -1 && side2.tab == 3, "sidebar fields");

        dst.pos = 0;
        memset(&song, 0, sizeof(song));
        song.id = -1;
        expect_err(rs_codec_song_encode(&s317, &dst, &song), RS_OK, "song encode");
        expect(dst.pos == 2 && dst.data[0] == 0xff && dst.data[1] == 0xff, "song id -1 is p2_le");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_song_decode(&s317, &src, &song2), RS_OK, "song decode");
        expect(song2.id == -1, "song id");

        dst.pos = 0;
        memset(&fr, 0, sizeof(fr));
        strcpy(fr.name, "zeb");
        fr.world = 2;
        expect_err(rs_codec_friend_entry_encode(&s317, &dst, &fr), RS_OK, "friend entry encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_friend_entry_decode(&s317, &src, &fr2), RS_OK, "friend entry decode");
        expect(strcmp(fr2.name, "zeb") == 0 && fr2.world == 2, "friend entry fields");

        dst.pos = 0;
        memset(&zone, 0, sizeof(zone));
        zone.x = 3;
        zone.z = 4;
        expect_err(rs_codec_zone_full_encode(&s317, &dst, &zone), RS_OK, "zone full encode");
        expect(dst.pos == 2 && dst.data[0] == 253 && dst.data[1] == 124, "zone full bytes");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_zone_full_decode(&s317, &src, &zone2), RS_OK, "zone full decode");
        expect(zone2.x == 3 && zone2.z == 4, "zone full fields");

        dst.pos = 0;
        memset(&loc, 0, sizeof(loc));
        loc.x = 1;
        loc.z = 2;
        loc.shape = 10;
        loc.rot = 1;
        loc.seq = 123;
        expect_err(rs_codec_loc_anim_encode(&s317, &dst, &loc), RS_OK, "loc anim encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_loc_anim_decode(&s317, &src, &loc2), RS_OK, "loc anim decode");
        expect(loc2.x == 1 && loc2.z == 2 && loc2.shape == 10 && loc2.rot == 1 && loc2.seq == 123,
               "loc anim fields");

        dst.pos = 0;
        memset(&hint, 0, sizeof(hint));
        hint.type = 1;
        hint.npc = 42;
        expect_err(rs_codec_hint_encode(&s317, &dst, &hint), RS_OK, "hint npc encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_hint_decode(&s317, &src, &hint2), RS_OK, "hint npc decode");
        expect(hint2.type == 1 && hint2.npc == 42, "hint npc fields");
        dst.pos = 0;
        memset(&hint, 0, sizeof(hint));
        hint.type = 3;
        hint.tile_x = 8;
        hint.tile_z = 9;
        hint.height = 2;
        expect_err(rs_codec_hint_encode(&s317, &dst, &hint), RS_OK, "hint tile encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_hint_decode(&s317, &src, &hint2), RS_OK, "hint tile decode");
        expect(hint2.type == 3 && hint2.tile_x == 8 && hint2.tile_z == 9 && hint2.height == 2,
               "hint type 3 stays 3");

        dst.pos = 0;
        memset(&invu, 0, sizeof(invu));
        invu.component = 10;
        invu.nchanges = 1;
        invu.changes[0].slot = 5;
        invu.changes[0].id = 20;
        invu.changes[0].amount = 300;
        expect_err(rs_codec_inv_update_encode(&s317, &dst, &invu), RS_OK, "inv update encode");
        expect(dst.pos == 10 && dst.data[0] == 0x00 && dst.data[1] == 10 && dst.data[2] == 0x05 &&
                   dst.data[3] == 0x00 && dst.data[4] == 0x14 && dst.data[5] == 255 &&
                   dst.data[6] == 0x00 && dst.data[7] == 0x00 && dst.data[8] == 0x01 &&
                   dst.data[9] == 0x2c,
               "inv update is p2 gsmarts p2 255 p4");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_inv_update_decode(&s317, &src, &invu2), RS_OK, "inv update decode");
        expect(invu2.component == 10 && invu2.nchanges == 1 && invu2.changes[0].slot == 5 &&
                   invu2.changes[0].id == 20 && invu2.changes[0].amount == 300,
               "inv update fields");

        dst.pos = 0;
        memset(&cmd, 0, sizeof(cmd));
        strcpy(cmd.text, "hello");
        expect_err(rs_codec_command_encode(&s317, &dst, &cmd), RS_OK, "command encode");
        nl = 0;
        for (i = 0; i < dst.pos; i++) {
            if (dst.data[i] == '\n') {
                nl = 1;
            }
        }
        expect(dst.pos > 0 && dst.data[0] == 'h' && nl, "command is a jagex string");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_command_decode(&s317, &src, &cmd2), RS_OK, "command decode");
        expect(strcmp(cmd2.text, "hello") == 0, "command text");

        memset(&map, 0, sizeof(map));
        for (p = 0; p < RS_INSTANCED_PLANES; p++) {
            for (z = 0; z < RS_INSTANCED_SIZE; z++) {
                for (x = 0; x < RS_INSTANCED_SIZE; x++) {
                    map.chunks[p][z][x] = -1;
                }
            }
        }
        map.chunks[0][0][0] = 5;
        map.zone_z = 10;
        map.zone_x = 20;
        dst.pos = 0;
        expect_err(rs_codec_region_instanced_encode(&s317, &dst, &map), RS_OK,
                   "instanced region encode");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_region_instanced_decode(&s317, &src, &map2), RS_OK,
                   "instanced region decode");
        expect(map2.chunks[0][0][0] == 5 && map2.zone_z == 10 && map2.zone_x == 20 &&
                   map2.chunks[0][0][1] == -1,
               "instanced region fields");

        dst.pos = 0;
        memset(&login, 0, sizeof(login));
        login.days_since_recovery = 2;
        login.unread = 4;
        login.members_warning = 1;
        login.address = 6;
        login.days_since_login = 8;
        expect_err(rs_codec_last_login_encode(&s317, &dst, &login), RS_OK, "last login encode");
        expect(dst.pos > 0 && dst.data[0] == 254, "last login recovery is p1_neg");
        rs_buf_wrap(&src, dst.data, dst.pos);
        expect_err(rs_codec_last_login_decode(&s317, &src, &login2), RS_OK, "last login decode");
        expect(login2.days_since_recovery == 2 && login2.unread == 4 &&
                   login2.members_warning == 1 && login2.address == 6 &&
                   login2.days_since_login == 8,
               "last login fields");

        rs_buf_free(&dst);
    }
    loopback_build("254");
    loopback_build("317");
    loopback_build("377");
    loopback_build("468");
    loopback_build("474");
    loopback_build("508");
    loopback_build("530");
    loopback_build("742");

    rs_ctx_destroy(ctx);
    if (g_fail) {
        fprintf(stderr, "%d test(s) failed\n", g_fail);
        return 1;
    }
    puts("ok");
    return 0;
}
