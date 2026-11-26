#include "rslib/both/codec.h"
#include "rslib/both/era_body.h"

#include <stdio.h>
#include <string.h>

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
    for (i = 0; i < b->pos && i < 32; i++) {
        fprintf(stderr, " %02x", b->data[i]);
    }
    fprintf(stderr, "\n");
    fails++;
}

static void match_bytes(const char *name, const rs_buf *a, const rs_buf *b) {
    if (a->pos != b->pos || memcmp(a->data, b->data, a->pos) != 0) {
        dump(name, a);
    }
}

/* Zone records are the opcode byte plus the codec body. */
static void match_zone(const char *name, const rs_buf *era, const rs_buf *codec, int opcode) {
    if (era->pos != codec->pos + 1 || era->data[0] != (uint8_t)opcode ||
        memcmp(era->data + 1, codec->data, codec->pos) != 0) {
        dump(name, era);
    }
}

static void wrap_src(rs_buf *src, uint8_t *storage, size_t n) {
    rs_buf_wrap(src, storage, n);
}

static void test_317_codec(void) {
    rs_session sess;
    uint8_t era_raw[256];
    uint8_t codec_raw[256];
    rs_buf era;
    rs_buf codec;
    rs_era_skill esk;
    rs_skill sk;
    rs_era_varp evp;
    rs_varp vp;
    rs_era_if_text etx;
    rs_if_text tx;
    rs_era_if_open eop;
    rs_if_open op;
    rs_era_if_side eside;
    rs_if_sub side;
    rs_era_if_top etop;
    rs_if_top top;
    rs_era_inventory einv;
    rs_inventory inv;
    rs_era_inv_update eupd;
    rs_inv_update upd;
    rs_era_loc eloc;
    rs_scene_loc loc;
    rs_era_obj eobj;
    rs_scene_obj obj;
    const uint8_t skill_wire[6] = {0x05, 0xe2, 0x40, 0x00, 0x01, 0x28};
    const uint8_t varp_small_wire[3] = {0x0c, 0x00, 0xfb};
    const uint8_t varp_large_wire[6] = {0x2c, 0x01, 0x0d, 0x40, 0x00, 0x03};
    const uint8_t text_wire[5] = {0x48, 0x69, 0x0a, 0x00, 0x15};
    const uint8_t inv_wire[7] = {0x00, 0x95, 0x00, 0x01, 0x03, 0x8a, 0x00};

    rs_session_init(&sess, rs_build_lookup(317));
    memset(&esk, 0, sizeof(esk));
    esk.id = 5;
    esk.level = 40;
    esk.xp = 123456;
    sk.id = esk.id;
    sk.level = esk.level;
    sk.xp = esk.xp;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 skill era", rs_era_skill_encode(317, &era, &esk), RS_OK);
    expect_err("317 skill codec", rs_codec_skill_encode(&sess, &codec, &sk), RS_OK);
    match_bytes("317 skill", &era, &codec);
    expect("317 skill known", era.pos == sizeof(skill_wire) &&
                                  memcmp(era.data, skill_wire, sizeof(skill_wire)) == 0);

    memset(&evp, 0, sizeof(evp));
    evp.id = 12;
    evp.value = -5;
    vp.id = evp.id;
    vp.value = evp.value;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 varp small era", rs_era_varp_small_encode(317, &era, &evp), RS_OK);
    expect_err("317 varp small codec", rs_codec_varp_encode(&sess, &codec, &vp), RS_OK);
    match_bytes("317 varp small", &era, &codec);
    expect("317 varp small known", era.pos == sizeof(varp_small_wire) &&
                                       memcmp(era.data, varp_small_wire, sizeof(varp_small_wire)) == 0);

    evp.id = 300;
    evp.value = 200000;
    vp.id = evp.id;
    vp.value = evp.value;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 varp large era", rs_era_varp_large_encode(317, &era, &evp), RS_OK);
    expect_err("317 varp large codec", rs_codec_varp_encode(&sess, &codec, &vp), RS_OK);
    match_bytes("317 varp large", &era, &codec);
    expect("317 varp large known", era.pos == sizeof(varp_large_wire) &&
                                       memcmp(era.data, varp_large_wire, sizeof(varp_large_wire)) == 0);

    memset(&etx, 0, sizeof(etx));
    memcpy(etx.text, "Hi", 3);
    etx.component = 149;
    memset(&tx, 0, sizeof(tx));
    memcpy(tx.text, "Hi", 3);
    tx.component = 149;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 text era", rs_era_if_text_encode(317, &era, &etx), RS_OK);
    expect_err("317 text codec", rs_codec_if_text_encode(&sess, &codec, &tx), RS_OK);
    match_bytes("317 text", &era, &codec);
    expect("317 text known", era.pos == sizeof(text_wire) &&
                                 memcmp(era.data, text_wire, sizeof(text_wire)) == 0);

    eop.id = 12;
    op.id = 12;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 open era", rs_era_if_open_encode(317, &era, &eop), RS_OK);
    expect_err("317 open codec", rs_codec_if_open_encode(&sess, &codec, &op), RS_OK);
    match_bytes("317 open", &era, &codec);

    memset(&eside, 0, sizeof(eside));
    eside.id = 149;
    side.id = 149;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 side era", rs_era_if_side_encode(317, &era, &eside), RS_OK);
    expect_err("317 side codec", rs_codec_if_sub_encode(&sess, &codec, &side), RS_OK);
    match_bytes("317 side", &era, &codec);

    memset(&etop, 0, sizeof(etop));
    etop.viewport = 548;
    etop.sidebar = 161;
    top.viewport = 548;
    top.sidebar = 161;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 top era", rs_era_if_top_encode(317, &era, &etop), RS_OK);
    expect_err("317 top codec", rs_codec_if_top_encode(&sess, &codec, &top), RS_OK);
    match_bytes("317 top", &era, &codec);

    memset(&einv, 0, sizeof(einv));
    einv.component = 149;
    einv.nslots = 1;
    einv.slots[0].id = 10;
    einv.slots[0].amount = 3;
    memset(&inv, 0, sizeof(inv));
    inv.component = 149;
    inv.nslots = 1;
    inv.slots[0].id = 10;
    inv.slots[0].amount = 3;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 inv era", rs_era_inventory_encode(317, &era, &einv), RS_OK);
    expect_err("317 inv codec", rs_codec_inventory_encode(&sess, &codec, &inv), RS_OK);
    match_bytes("317 inv", &era, &codec);
    expect("317 inv known", era.pos == sizeof(inv_wire) &&
                                memcmp(era.data, inv_wire, sizeof(inv_wire)) == 0);

    einv.nslots = 2;
    einv.slots[0].id = -4;
    einv.slots[0].amount = -9;
    einv.slots[1].id = 995;
    einv.slots[1].amount = 1000;
    inv.nslots = 2;
    inv.slots[0].id = -4;
    inv.slots[0].amount = -9;
    inv.slots[1].id = 995;
    inv.slots[1].amount = 1000;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 inv clamp era", rs_era_inventory_encode(317, &era, &einv), RS_OK);
    expect_err("317 inv clamp codec", rs_codec_inventory_encode(&sess, &codec, &inv), RS_OK);
    match_bytes("317 inv clamp", &era, &codec);

    memset(&eupd, 0, sizeof(eupd));
    eupd.component = 3214;
    eupd.nchanges = 2;
    eupd.changes[0].slot = 5;
    eupd.changes[0].id = 4151;
    eupd.changes[0].amount = 1;
    eupd.changes[1].slot = 200;
    eupd.changes[1].id = 995;
    eupd.changes[1].amount = 1000;
    memset(&upd, 0, sizeof(upd));
    upd.component = 3214;
    upd.nchanges = 2;
    upd.changes[0].slot = 5;
    upd.changes[0].id = 4151;
    upd.changes[0].amount = 1;
    upd.changes[1].slot = 200;
    upd.changes[1].id = 995;
    upd.changes[1].amount = 1000;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 upd era", rs_era_inv_update_encode(317, &era, &eupd), RS_OK);
    expect_err("317 upd codec", rs_codec_inv_update_encode(&sess, &codec, &upd), RS_OK);
    match_bytes("317 upd", &era, &codec);

    memset(&eloc, 0, sizeof(eloc));
    eloc.x = 3;
    eloc.z = 5;
    eloc.id = 1285;
    eloc.shape = 10;
    eloc.rot = 2;
    loc.x = 3;
    loc.z = 5;
    loc.id = 1285;
    loc.shape = 10;
    loc.rot = 2;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 loc add era", rs_era_loc_add_encode(317, &era, &eloc), RS_OK);
    expect_err("317 loc add codec", rs_codec_loc_add_encode(&sess, &codec, &loc), RS_OK);
    match_zone("317 loc add", &era, &codec, 151);

    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 loc del era", rs_era_loc_del_encode(317, &era, &eloc), RS_OK);
    expect_err("317 loc del codec", rs_codec_loc_del_encode(&sess, &codec, &loc), RS_OK);
    match_zone("317 loc del", &era, &codec, 101);

    memset(&eobj, 0, sizeof(eobj));
    eobj.x = 1;
    eobj.z = 6;
    eobj.id = 995;
    eobj.amount = 50;
    obj.x = 1;
    obj.z = 6;
    obj.id = 995;
    obj.amount = 50;
    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 obj add era", rs_era_obj_add_encode(317, &era, &eobj), RS_OK);
    expect_err("317 obj add codec", rs_codec_obj_add_encode(&sess, &codec, &obj), RS_OK);
    match_zone("317 obj add", &era, &codec, 44);

    rs_buf_wrap(&era, era_raw, sizeof(era_raw));
    rs_buf_wrap(&codec, codec_raw, sizeof(codec_raw));
    expect_err("317 obj del era", rs_era_obj_del_encode(317, &era, &eobj), RS_OK);
    expect_err("317 obj del codec", rs_codec_obj_del_encode(&sess, &codec, &obj), RS_OK);
    match_zone("317 obj del", &era, &codec, 156);
}

static void rt_skill(int rev) {
    uint8_t storage[32];
    rs_buf dst;
    rs_buf src;
    rs_era_skill in;
    rs_era_skill out;
    memset(&in, 0, sizeof(in));
    in.id = 5;
    in.level = 40;
    in.xp = 123456;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("skill enc", rev), rs_era_skill_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("skill dec", rev), rs_era_skill_decode(rev, &src, &out), RS_OK);
    expect(nm("skill val", rev), out.id == 5 && out.level == 40 && out.xp == 123456);
    expect(nm("skill left", rev), rs_buf_remaining(&src) == 0);
}

static void rt_varp(int rev) {
    uint8_t storage[16];
    rs_buf dst;
    rs_buf src;
    rs_era_varp in;
    rs_era_varp out;
    memset(&in, 0, sizeof(in));
    in.id = 12;
    in.value = -5;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("varp small enc", rev), rs_era_varp_small_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("varp small dec", rev), rs_era_varp_small_decode(rev, &src, &out), RS_OK);
    expect(nm("varp small val", rev), out.id == 12 && out.value == -5);
    expect(nm("varp small left", rev), rs_buf_remaining(&src) == 0);

    in.id = 300;
    in.value = rev == 910 ? -40000 : 200000;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("varp large enc", rev), rs_era_varp_large_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("varp large dec", rev), rs_era_varp_large_decode(rev, &src, &out), RS_OK);
    expect(nm("varp large val", rev), out.id == 300 && out.value == in.value);
    expect(nm("varp large left", rev), rs_buf_remaining(&src) == 0);
}

static void rt_inventory(int rev) {
    uint8_t storage[64];
    rs_buf dst;
    rs_buf src;
    rs_era_inventory in;
    rs_era_inventory out;
    int flags = (rev == 742 || rev == 910) ? 1 : 0;
    memset(&in, 0, sizeof(in));
    in.component = 149;
    in.flags = flags;
    in.nslots = 2;
    in.slots[0].id = 4151;
    in.slots[0].amount = 1;
    in.slots[1].id = 995;
    in.slots[1].amount = 1000;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("inv enc", rev), rs_era_inventory_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("inv dec", rev), rs_era_inventory_decode(rev, &src, &out), RS_OK);
    expect(nm("inv val", rev),
           out.component == 149 && out.flags == flags && out.nslots == 2 &&
               out.slots[0].id == 4151 && out.slots[0].amount == 1 && out.slots[1].id == 995 &&
               out.slots[1].amount == 1000);
    expect(nm("inv left", rev), rs_buf_remaining(&src) == 0);
}

static void rt_update(int rev) {
    uint8_t storage[64];
    rs_buf dst;
    rs_buf src;
    rs_era_inv_update in;
    rs_era_inv_update out;
    int late = rev == 742 || rev == 910;
    int flags = late ? 1 : 0;
    memset(&in, 0, sizeof(in));
    in.component = 3214;
    in.flags = flags;
    in.nchanges = late ? 3 : 2;
    in.changes[0].slot = 5;
    in.changes[0].id = 4151;
    in.changes[0].amount = 1;
    in.changes[1].slot = rev == 254 ? 40 : 200;
    in.changes[1].id = 995;
    in.changes[1].amount = 1000;
    if (late) {
        in.changes[2].slot = 7;
        in.changes[2].id = -1;
        in.changes[2].amount = 0;
    }
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("upd enc", rev), rs_era_inv_update_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("upd dec", rev), rs_era_inv_update_decode(rev, &src, &out), RS_OK);
    expect(nm("upd val", rev),
           out.component == 3214 && out.flags == flags && out.nchanges == in.nchanges &&
               out.changes[0].slot == 5 && out.changes[0].id == 4151 && out.changes[0].amount == 1 &&
               out.changes[1].id == 995 && out.changes[1].amount == 1000 &&
               out.changes[1].slot == in.changes[1].slot);
    if (late) {
        expect(nm("upd del", rev), out.changes[2].slot == 7 && out.changes[2].id == -1 &&
                                       out.changes[2].amount == 0);
    }
    expect(nm("upd left", rev), rs_buf_remaining(&src) == 0);
}

static void rt_text(int rev) {
    uint8_t storage[64];
    rs_buf dst;
    rs_buf src;
    rs_era_if_text in;
    rs_era_if_text out;
    memset(&in, 0, sizeof(in));
    memcpy(in.text, "Hello", 6);
    in.component = 149;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("text enc", rev), rs_era_if_text_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("text dec", rev), rs_era_if_text_decode(rev, &src, &out), RS_OK);
    expect(nm("text val", rev), out.component == 149 && strcmp(out.text, "Hello") == 0);
    expect(nm("text left", rev), rs_buf_remaining(&src) == 0);
}

static void rt_open(int rev) {
    uint8_t storage[8];
    rs_buf dst;
    rs_buf src;
    rs_era_if_open in;
    rs_era_if_open out;
    in.id = 165;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("open enc", rev), rs_era_if_open_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("open dec", rev), rs_era_if_open_decode(rev, &src, &out), RS_OK);
    expect(nm("open val", rev), out.id == 165);
    expect(nm("open left", rev), rs_buf_remaining(&src) == 0);
}

static void rt_side(int rev) {
    uint8_t storage[32];
    rs_buf dst;
    rs_buf src;
    rs_era_if_side in;
    rs_era_if_side out;
    int late = rev == 742 || rev == 910;
    memset(&in, 0, sizeof(in));
    in.id = 149;
    if (late) {
        in.parent = 500;
        in.type = rev == 910 ? 7 : 9;
        in.key[0] = 11;
        in.key[1] = -22;
        in.key[2] = 33;
        in.key[3] = -44;
    }
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("side enc", rev), rs_era_if_side_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("side dec", rev), rs_era_if_side_decode(rev, &src, &out), RS_OK);
    expect(nm("side id", rev), out.id == 149);
    if (late) {
        expect(nm("side rest", rev),
               out.parent == in.parent && out.type == in.type && out.key[0] == 11 &&
                   out.key[1] == -22 && out.key[2] == 33 && out.key[3] == -44);
        expect(nm("side len", rev), dst.pos == 23);
    }
    expect(nm("side left", rev), rs_buf_remaining(&src) == 0);
}

static void rt_top(int rev) {
    uint8_t storage[32];
    rs_buf dst;
    rs_buf src;
    rs_era_if_top in;
    rs_era_if_top out;
    int late = rev == 742 || rev == 910;
    memset(&in, 0, sizeof(in));
    in.viewport = 548;
    in.sidebar = late ? 0 : 161;
    if (late) {
        in.type = 4;
        in.key[0] = 11;
        in.key[1] = -22;
        in.key[2] = 33;
        in.key[3] = -44;
    }
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("top enc", rev), rs_era_if_top_encode(rev, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("top dec", rev), rs_era_if_top_decode(rev, &src, &out), RS_OK);
    expect(nm("top view", rev), out.viewport == 548);
    if (late) {
        expect(nm("top keys", rev),
               out.type == 4 && out.key[0] == 11 && out.key[1] == -22 && out.key[2] == 33 &&
                   out.key[3] == -44);
        expect(nm("top len", rev), dst.pos == 19);
    } else {
        expect(nm("top side", rev), out.sidebar == 161);
    }
    expect(nm("top left", rev), rs_buf_remaining(&src) == 0);
}

static void rt_close(int rev) {
    uint8_t storage[8];
    rs_buf dst;
    rs_buf src;
    rs_era_if_close in;
    rs_era_if_close out;
    int early = rev == 254 || rev == 317 || rev == 350 || rev == 377 || rev == 474;
    memset(&in, 0, sizeof(in));
    in.component = early ? 99 : 0x01020304;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("close enc", rev), rs_era_if_close_encode(rev, &dst, &in), RS_OK);
    if (early) {
        expect(nm("close empty", rev), dst.pos == 0);
    }
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("close dec", rev), rs_era_if_close_decode(rev, &src, &out), RS_OK);
    expect(nm("close val", rev), out.component == (early ? 0 : 0x01020304));
}

static void rt_zones(int rev) {
    uint8_t storage[32];
    rs_buf dst;
    rs_buf src;
    rs_era_loc loc;
    rs_era_loc loc2;
    rs_era_obj obj;
    rs_era_obj obj2;
    memset(&loc, 0, sizeof(loc));
    loc.x = 3;
    loc.z = 5;
    loc.id = 1285;
    loc.shape = 10;
    loc.rot = 2;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("loc add enc", rev), rs_era_loc_add_encode(rev, &dst, &loc), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("loc add dec", rev), rs_era_loc_add_decode(rev, &src, &loc2), RS_OK);
    expect(nm("loc add val", rev),
           loc2.x == 3 && loc2.z == 5 && loc2.id == 1285 && loc2.shape == 10 && loc2.rot == 2);
    expect(nm("loc add left", rev), rs_buf_remaining(&src) == 0);

    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("loc del enc", rev), rs_era_loc_del_encode(rev, &dst, &loc), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("loc del dec", rev), rs_era_loc_del_decode(rev, &src, &loc2), RS_OK);
    expect(nm("loc del val", rev),
           loc2.x == 3 && loc2.z == 5 && loc2.id == -1 && loc2.shape == 10 && loc2.rot == 2);
    expect(nm("loc del left", rev), rs_buf_remaining(&src) == 0);

    memset(&obj, 0, sizeof(obj));
    obj.x = 1;
    obj.z = 6;
    obj.id = 995;
    obj.amount = 50;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("obj add enc", rev), rs_era_obj_add_encode(rev, &dst, &obj), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("obj add dec", rev), rs_era_obj_add_decode(rev, &src, &obj2), RS_OK);
    expect(nm("obj add val", rev), obj2.x == 1 && obj2.z == 6 && obj2.id == 995 && obj2.amount == 50);
    expect(nm("obj add left", rev), rs_buf_remaining(&src) == 0);

    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err(nm("obj del enc", rev), rs_era_obj_del_encode(rev, &dst, &obj), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err(nm("obj del dec", rev), rs_era_obj_del_decode(rev, &src, &obj2), RS_OK);
    expect(nm("obj del val", rev), obj2.x == 1 && obj2.z == 6 && obj2.id == 995);
    expect(nm("obj del left", rev), rs_buf_remaining(&src) == 0);
}

/* JavaMouseWheel.call72b59b7f opcodes 119 and 187. */
static void test_474_inv(void) {
    uint8_t storage[64];
    rs_buf dst;
    rs_buf src;
    rs_era_inventory in;
    rs_era_inventory out;
    rs_era_inv_update uin;
    rs_era_inv_update uout;
    memset(&in, 0, sizeof(in));
    in.component = -70001;
    in.flags = 40000;
    in.nslots = 2;
    in.slots[0].id = 4151;
    in.slots[0].amount = 1;
    in.slots[1].id = 0;
    in.slots[1].amount = 1000;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err("474 inv enc", rs_era_inventory_encode(474, &dst, &in), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err("474 inv dec", rs_era_inventory_decode(474, &src, &out), RS_OK);
    expect("474 inv val", out.component == -70001 && out.flags == 40000 && out.nslots == 2 &&
                              out.slots[0].id == 4151 && out.slots[0].amount == 1 &&
                              out.slots[1].id == 0 && out.slots[1].amount == 1000);
    expect("474 inv left", rs_buf_remaining(&src) == 0);

    memset(&uin, 0, sizeof(uin));
    uin.component = 3214;
    uin.flags = 7;
    uin.nchanges = 2;
    uin.changes[0].slot = 5;
    uin.changes[0].id = 0;
    uin.changes[0].amount = 9;
    uin.changes[1].slot = 200;
    uin.changes[1].id = 995;
    uin.changes[1].amount = 1000;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err("474 upd enc", rs_era_inv_update_encode(474, &dst, &uin), RS_OK);
    wrap_src(&src, storage, dst.pos);
    expect_err("474 upd dec", rs_era_inv_update_decode(474, &src, &uout), RS_OK);
    expect("474 upd val", uout.component == 3214 && uout.flags == 7 && uout.nchanges == 2 &&
                              uout.changes[0].slot == 5 && uout.changes[0].id == 0 &&
                              uout.changes[0].amount == 0 && uout.changes[1].slot == 200 &&
                              uout.changes[1].id == 995 && uout.changes[1].amount == 1000);
    expect("474 upd left", rs_buf_remaining(&src) == 0);
}

static void test_roundtrips(void) {
    static const int full[] = {317, 254, 350, 377, 742, 910};
    static const int skill[] = {317, 254, 350, 377, 508, 742, 910};
    static const int opens[] = {317, 254, 350, 377};
    size_t i;
    uint8_t a[16];
    uint8_t b[16];
    rs_buf da;
    rs_buf db;
    rs_era_skill sk;
    for (i = 0; i < sizeof(skill) / sizeof(skill[0]); i++) {
        rt_skill(skill[i]);
        rt_varp(skill[i]);
    }
    for (i = 0; i < sizeof(full) / sizeof(full[0]); i++) {
        rt_inventory(full[i]);
        rt_update(full[i]);
        rt_text(full[i]);
        rt_side(full[i]);
        rt_top(full[i]);
        rt_close(full[i]);
        rt_zones(full[i]);
    }
    for (i = 0; i < sizeof(opens) / sizeof(opens[0]); i++) {
        rt_open(opens[i]);
    }
    memset(&sk, 0, sizeof(sk));
    sk.id = 5;
    sk.level = 40;
    sk.xp = 123456;
    rs_buf_wrap(&da, a, sizeof(a));
    rs_buf_wrap(&db, b, sizeof(b));
    expect_err("254 skill", rs_era_skill_encode(254, &da, &sk), RS_OK);
    expect_err("317 skill again", rs_era_skill_encode(317, &db, &sk), RS_OK);
    expect("254 skill differs", da.pos != db.pos || memcmp(a, b, da.pos) != 0);
    rt_skill(474);
    rt_varp(474);
    rt_close(474);
    test_474_inv();
}

static void test_refuse(void) {
    uint8_t storage[32];
    uint8_t bad_op[4] = {0xff, 0x00, 0x00, 0x00};
    uint8_t scaled_add[2] = {10, 128};
    uint8_t scaled_del[2] = {5, 128};
    uint8_t trunc[5] = {0x00, 0x95, 0x00, 0x01, 0xff};
    uint8_t del_only[8];
    rs_buf dst;
    rs_buf src;
    rs_era_skill sk;
    rs_era_varp vp;
    rs_era_inventory inv;
    rs_era_if_open op;
    rs_era_if_text tx;
    rs_era_loc loc;
    rs_era_inv_update upd;
    rs_era_inv_update upd2;
    memset(&sk, 0, sizeof(sk));
    sk.id = 1;
    sk.level = 1;
    sk.xp = 1;
    memset(&vp, 0, sizeof(vp));
    memset(&inv, 0, sizeof(inv));
    inv.nslots = 1;
    memset(&op, 0, sizeof(op));
    memset(&tx, 0, sizeof(tx));
    memset(&loc, 0, sizeof(loc));
    rs_buf_wrap(&dst, storage, sizeof(storage));

    expect_err("null buf", rs_era_skill_encode(831, NULL, &sk), RS_ERR_ARG);
    expect_err("null val", rs_era_skill_encode(831, &dst, NULL), RS_ERR_ARG);
    expect_err("831 skill", rs_era_skill_encode(831, &dst, &sk), RS_ERR_UNSUPPORTED);
    expect_err("831 loc", rs_era_loc_add_encode(831, &dst, &loc), RS_ERR_UNSUPPORTED);
    expect_err("860 skill", rs_era_skill_encode(860, &dst, &sk), RS_ERR_UNSUPPORTED);
    expect_err("474 text", rs_era_if_text_encode(474, &dst, &tx), RS_ERR_UNSUPPORTED);
    expect_err("474 zone", rs_era_obj_add_encode(474, &dst, &(rs_era_obj){0}), RS_ERR_UNSUPPORTED);
    expect_err("530 skill", rs_era_skill_encode(530, &dst, &sk), RS_ERR_UNSUPPORTED);
    expect_err("530 varp", rs_era_varp_small_encode(530, &dst, &vp), RS_ERR_UNSUPPORTED);
    expect_err("530 open", rs_era_if_open_encode(530, &dst, &op), RS_ERR_UNSUPPORTED);
    expect_err("530 zone", rs_era_loc_del_encode(530, &dst, &loc), RS_ERR_UNSUPPORTED);
    expect_err("508 inv", rs_era_inventory_encode(508, &dst, &inv), RS_ERR_UNSUPPORTED);
    expect_err("508 upd", rs_era_inv_update_encode(508, &dst, &(rs_era_inv_update){0}), RS_ERR_UNSUPPORTED);
    expect_err("508 text", rs_era_if_text_encode(508, &dst, &tx), RS_ERR_UNSUPPORTED);
    expect_err("508 open", rs_era_if_open_encode(508, &dst, &op), RS_ERR_UNSUPPORTED);
    expect_err("508 side", rs_era_if_side_encode(508, &dst, &(rs_era_if_side){0}), RS_ERR_UNSUPPORTED);
    expect_err("508 top", rs_era_if_top_encode(508, &dst, &(rs_era_if_top){0}), RS_ERR_UNSUPPORTED);
    expect_err("508 close", rs_era_if_close_encode(508, &dst, &(rs_era_if_close){0}), RS_ERR_UNSUPPORTED);
    expect_err("508 loc", rs_era_loc_add_encode(508, &dst, &loc), RS_ERR_UNSUPPORTED);
    expect_err("742 open", rs_era_if_open_encode(742, &dst, &op), RS_ERR_UNSUPPORTED);
    expect_err("910 open", rs_era_if_open_decode(910, &dst, &op), RS_ERR_UNSUPPORTED);

    inv.flags = 65536;
    inv.nslots = 0;
    expect_err("474 flags", rs_era_inventory_encode(474, &dst, &inv), RS_ERR_ARG);
    inv.flags = 2;
    inv.nslots = 0;
    expect_err("910 flags", rs_era_inventory_encode(910, &dst, &inv), RS_ERR_UNSUPPORTED);
    expect_err("early flags", rs_era_inventory_encode(317, &dst, &inv), RS_ERR_ARG);
    inv.flags = 0;
    inv.nslots = 256;
    expect_err("254 slots", rs_era_inventory_encode(254, &dst, &inv), RS_ERR_ARG);
    {
        uint8_t big[4096];
        rs_buf wide;
        rs_buf_wrap(&wide, big, sizeof big);
        inv.nslots = 304;
        inv.slots[303].id = 7;
        inv.slots[303].amount = 1;
        expect_err("317 304", rs_era_inventory_encode(317, &wide, &inv), RS_OK);
        expect_err("254 304", rs_era_inventory_encode(254, &wide, &inv), RS_ERR_ARG);
    }

    wrap_src(&src, bad_op, sizeof(bad_op));
    expect_err("bad opcode", rs_era_loc_add_decode(317, &src, &loc), RS_ERR_FORMAT);
    wrap_src(&src, scaled_add, sizeof(scaled_add));
    expect_err("910 scale add", rs_era_loc_add_decode(910, &src, &loc), RS_ERR_UNSUPPORTED);
    wrap_src(&src, scaled_del, sizeof(scaled_del));
    expect_err("910 scale del", rs_era_loc_del_decode(910, &src, &loc), RS_ERR_UNSUPPORTED);
    wrap_src(&src, trunc, sizeof(trunc));
    expect_err("trunc inv", rs_era_inventory_decode(317, &src, &inv), RS_ERR_FORMAT);

    memset(&upd, 0, sizeof(upd));
    upd.component = 3214;
    upd.flags = 1;
    upd.nchanges = 1;
    upd.changes[0].slot = 4;
    upd.changes[0].id = -1;
    upd.changes[0].amount = 0;
    rs_buf_wrap(&dst, del_only, sizeof(del_only));
    expect_err("742 del enc", rs_era_inv_update_encode(742, &dst, &upd), RS_OK);
    expect("742 del len", dst.pos == 6);
    wrap_src(&src, del_only, dst.pos);
    expect_err("742 del dec", rs_era_inv_update_decode(742, &src, &upd2), RS_OK);
    expect("742 del id", upd2.nchanges == 1 && upd2.changes[0].id == -1 && upd2.changes[0].amount == 0);
}

int main(void) {
    test_317_codec();
    test_roundtrips();
    test_refuse();
    if (fails) {
        fprintf(stderr, "%d checks failed\n", fails);
        return 1;
    }
    return 0;
}
