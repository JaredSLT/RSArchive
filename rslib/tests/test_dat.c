#include "rslib/both/dat.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_failed;

static void expect(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "fail: %s\n", msg);
        g_failed = 1;
    }
}

static void expect_err(rs_err e, rs_err want, const char *msg) {
    if (e != want) {
        fprintf(stderr, "fail: %s (got %d want %d)\n", msg, (int)e, (int)want);
        g_failed = 1;
    }
}

typedef struct tb {
    uint8_t *d;
    size_t n;
    size_t cap;
} tb;

static void tb_need(tb *t, size_t n) {
    size_t c;
    uint8_t *p;
    if (t->n + n <= t->cap) {
        return;
    }
    c = t->cap ? t->cap * 2 : 64;
    while (c < t->n + n) {
        c *= 2;
    }
    p = realloc(t->d, c);
    if (!p) {
        fprintf(stderr, "fail: oom\n");
        exit(1);
    }
    t->d = p;
    t->cap = c;
}

static void u1(tb *t, int v) {
    tb_need(t, 1);
    t->d[t->n++] = (uint8_t)v;
}

static void u2(tb *t, int v) {
    u1(t, v >> 8);
    u1(t, v);
}

static void u4(tb *t, uint32_t v) {
    u1(t, (int)(v >> 24));
    u1(t, (int)(v >> 16));
    u1(t, (int)(v >> 8));
    u1(t, (int)v);
}

static void u8(tb *t, uint64_t v) {
    int i;
    for (i = 7; i >= 0; i--) {
        u1(t, (int)((v >> (i * 8)) & 0xff));
    }
}

static void raw_put(tb *t, const void *p, size_t n) {
    tb_need(t, n);
    memcpy(t->d + t->n, p, n);
    t->n += n;
}

static void tb_free(tb *t) {
    free(t->d);
    t->d = NULL;
    t->n = t->cap = 0;
}

static void script_seal(tb *t, int opcount) {
    int i;
    u4(t, (uint32_t)opcount);
    for (i = 0; i < 6; i++) {
        u2(t, 0);
    }
    u1(t, 0);
    u2(t, 1);
}

static void test_varbit(void) {
    uint8_t raw[] = {1, 0, 10, 3, 7, 10, 'a', 'b', 10, 2, 0};
    uint8_t bad[] = {9};
    uint8_t late[] = {1, 0, 4, 1, 6, 0};
    uint8_t unk[] = {2, 0};
    uint8_t v910[] = {1, 0, 0, 5, 2, 3, 9, 11, 0};
    uint8_t smart[] = {1, 0, 0x7f, 0xff, 0};
    uint8_t dom[] = {1, 11, 0, 5, 0};
    uint8_t name[] = {11, 0};
    uint8_t serial[] = {99};
    uint8_t empty[] = {0};
    rs_buf buf;
    rs_varbit v;
    int revs[] = {474, 530, 831, 860};
    int i;
    rs_buf_wrap(&buf, raw, sizeof(raw));
    expect_err(rs_varbit_decode(317, &buf, &v), RS_OK, "317 varbit");
    expect(v.basevar == 10 && v.startbit == 3 && v.endbit == 7, "317 varbit bits");
    expect(v.op2 == 1 && v.op3 == -1 && v.name && strcmp(v.name, "ab") == 0, "317 varbit name");
    rs_varbit_free(&v);
    rs_buf_wrap(&buf, bad, sizeof(bad));
    expect_err(rs_varbit_decode(317, &buf, &v), RS_ERR_FORMAT, "317 varbit opcode");
    rs_varbit_free(&v);
    rs_buf_wrap(&buf, late, sizeof(late));
    expect_err(rs_varbit_decode(742, &buf, &v), RS_OK, "742 varbit");
    expect(v.basevar == 4 && v.startbit == 1 && v.endbit == 6 && v.name == NULL, "742 varbit fields");
    rs_varbit_free(&v);
    rs_buf_wrap(&buf, unk, sizeof(unk));
    expect_err(rs_varbit_decode(742, &buf, &v), RS_ERR_FORMAT, "742 varbit opcode");
    rs_buf_wrap(&buf, v910, sizeof(v910));
    expect_err(rs_varbit_decode(910, &buf, &v), RS_OK, "910 varbit");
    expect(v.basevar == 5 && v.domain == 0 && v.domain_set == 1, "910 varbit base");
    expect(v.startbit == 3 && v.endbit == 9 && v.name == NULL, "910 varbit bits");
    rs_varbit_free(&v);
    rs_buf_wrap(&buf, smart, sizeof(smart));
    expect_err(rs_varbit_decode(910, &buf, &v), RS_OK, "910 varbit smart");
    expect(v.basevar == -1 && v.domain == 0, "910 varbit null smart");
    rs_varbit_free(&v);
    rs_buf_wrap(&buf, dom, sizeof(dom));
    expect_err(rs_varbit_decode(910, &buf, &v), RS_ERR_FORMAT, "910 varbit domain");
    rs_buf_wrap(&buf, name, sizeof(name));
    expect_err(rs_varbit_decode(910, &buf, &v), RS_OK, "910 varbit debugname");
    expect(v.basevar == -1 && v.domain_set == 0, "910 debugname unread");
    rs_varbit_free(&v);
    rs_buf_wrap(&buf, serial, sizeof(serial));
    expect_err(rs_varbit_decode(910, &buf, &v), RS_ERR_FORMAT, "910 varbit serial");
    rs_buf_wrap(&buf, empty, 1);
    for (i = 0; i < 4; i++) {
        expect_err(rs_varbit_decode(revs[i], &buf, &v), RS_ERR_UNSUPPORTED, "varbit revision");
    }
    expect_err(rs_varbit_decode(831, NULL, &v), RS_ERR_ARG, "varbit null src");
    expect_err(rs_varbit_decode(317, &buf, NULL), RS_ERR_ARG, "varbit null out");
}

static void test_enum(void) {
    uint8_t raw[] = {1, 'i', 2, 1, 4, 0, 0, 0, 7, 5, 0, 1, 0, 0, 0, 3, 'a', 'b', 0, 0};
    uint8_t zero[] = {1, 0};
    uint8_t euro[] = {1, 128, 0};
    uint8_t smart[] = {101, 4, 0};
    uint8_t empty[] = {0};
    rs_buf buf;
    rs_enum e;
    rs_buf_wrap(&buf, raw, sizeof(raw));
    expect_err(rs_enum_decode(742, &buf, &e), RS_OK, "742 enum");
    expect(e.input_char == 'i' && e.output_char == 1 && e.default_int == 7, "742 enum types");
    expect(e.default_str && strcmp(e.default_str, "null") == 0, "742 enum null");
    expect(e.count == 1 && e.entries && e.entries[0].key == 3, "742 enum key");
    expect(e.entries[0].is_string && strcmp(e.entries[0].str_value, "ab") == 0, "742 enum str");
    rs_enum_free(&e);
    rs_buf_wrap(&buf, zero, sizeof(zero));
    expect_err(rs_enum_decode(742, &buf, &e), RS_ERR_FORMAT, "742 enum char 0");
    rs_buf_wrap(&buf, euro, sizeof(euro));
    expect_err(rs_enum_decode(742, &buf, &e), RS_OK, "742 enum cp1252");
    expect(e.input_char == 0x20ac, "742 euro");
    rs_enum_free(&e);
    rs_buf_wrap(&buf, smart, sizeof(smart));
    expect_err(rs_enum_decode(742, &buf, &e), RS_ERR_FORMAT, "742 enum 101");
    rs_buf_wrap(&buf, smart, sizeof(smart));
    expect_err(rs_enum_decode(910, &buf, &e), RS_OK, "910 enum smart");
    expect(e.input_type == 4 && e.input_type_set == 1, "910 enum type");
    rs_enum_free(&e);
    rs_buf_wrap(&buf, empty, 1);
    expect_err(rs_enum_decode(317, &buf, &e), RS_ERR_UNSUPPORTED, "317 enum");
    expect_err(rs_enum_decode(474, &buf, &e), RS_ERR_UNSUPPORTED, "474 enum");
    expect_err(rs_enum_decode(530, &buf, &e), RS_ERR_UNSUPPORTED, "530 enum");
    expect_err(rs_enum_decode(860, &buf, &e), RS_ERR_UNSUPPORTED, "860 enum");
    expect_err(rs_enum_decode(742, NULL, &e), RS_ERR_ARG, "enum null");
}

static void test_seq(void) {
    uint8_t empty[] = {0};
    uint8_t op1[] = {1, 1, 0, 2, 0xff, 0xff, 0, 3, 0};
    uint8_t op3[] = {3, 1, 4, 0};
    uint8_t late[] = {1, 0, 1, 0, 5, 0, 2, 0, 1, 0};
    uint8_t noop[] = {16, 0};
    uint8_t bad[] = {4, 0};
    uint8_t flag[] = {18, 0};
    uint8_t nosnd[] = {19, 0, 1};
    uint8_t snd[] = {13, 0, 1, 0, 19, 0, 9, 0};
    uint8_t pair[] = {13, 0, 1, 0, 20, 0, 0, 3, 0, 4, 0};
    uint8_t wide[] = {13, 0, 1, 0, 119, 0, 0, 7, 0};
    uint8_t drop[] = {23, 0, 7, 0};
    uint8_t z[] = {0};
    rs_buf buf;
    rs_seq s;
    rs_buf_wrap(&buf, empty, 1);
    expect_err(rs_seq_decode(317, &buf, &s), RS_OK, "317 seq empty");
    expect(s.frame_count == 1 && s.frames[0] == -1 && s.iframes[0] == -1 && s.delays[0] == -1, "317 dummy");
    expect(s.preanim_move == 0 && s.postanim_move == 0 && s.priority == 5, "317 seq moves");
    expect(s.replaycount == 99 && s.replacemode == 2 && s.righthand == -1 && s.replayoff == -1, "317 seq defaults");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, op1, sizeof(op1));
    expect_err(rs_seq_decode(317, &buf, &s), RS_OK, "317 seq frame");
    expect(s.frame_count == 1 && s.frames[0] == 2 && s.iframes[0] == -1 && s.delays[0] == 3, "317 seq op1");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, op3, sizeof(op3));
    expect_err(rs_seq_decode(317, &buf, &s), RS_OK, "317 seq walk");
    expect(s.walkmerge_count == 2 && s.walkmerge[0] == 4 && s.walkmerge[1] == 9999999, "317 walkmerge");
    expect(s.preanim_move == 2 && s.postanim_move == 2, "317 walk moves");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, late, sizeof(late));
    expect_err(rs_seq_decode(742, &buf, &s), RS_OK, "742 seq");
    expect(s.frame_count == 1 && s.frames[0] == (2 + (1 << 16)) && s.delays[0] == 5, "742 packed");
    expect(s.preanim_move == 0 && s.postanim_move == 0 && s.length == 0, "742 moves");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, late, sizeof(late));
    expect_err(rs_seq_decode(910, &buf, &s), RS_OK, "910 seq");
    expect(s.length == 5 && s.preanim_move == 0 && s.postanim_move == 0, "910 length");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, noop, sizeof(noop));
    expect_err(rs_seq_decode(742, &buf, &s), RS_OK, "742 seq 16");
    expect(s.frame_count == 0 && s.frames == NULL, "742 no dummy");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, bad, sizeof(bad));
    expect_err(rs_seq_decode(742, &buf, &s), RS_ERR_FORMAT, "742 seq 4");
    rs_buf_wrap(&buf, flag, sizeof(flag));
    expect_err(rs_seq_decode(742, &buf, &s), RS_OK, "742 seq 18");
    expect(s.flag18 == 1, "742 flag18");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, flag, sizeof(flag));
    expect_err(rs_seq_decode(910, &buf, &s), RS_OK, "910 seq 18");
    expect(s.flag18 == 0, "910 opcode 18 noop");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, nosnd, sizeof(nosnd));
    expect_err(rs_seq_decode(742, &buf, &s), RS_ERR_FORMAT, "742 seq 19");
    rs_buf_wrap(&buf, snd, sizeof(snd));
    expect_err(rs_seq_decode(742, &buf, &s), RS_OK, "742 seq sound byte");
    expect(s.sound_count == 1 && s.sound_bytes && s.sound_bytes[0] == 9, "742 sound byte");
    expect(s.sounds && s.sounds[0] == NULL && s.sound_a == NULL, "742 empty row");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, pair, sizeof(pair));
    expect_err(rs_seq_decode(742, &buf, &s), RS_OK, "742 seq sound pair");
    expect(s.sound_a[0] == 3 && s.sound_b[0] == 4 && s.sound_bytes == NULL, "742 sound pair");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, wide, sizeof(wide));
    expect_err(rs_seq_decode(910, &buf, &s), RS_OK, "910 seq 119");
    expect(s.sound_bytes[0] == 7, "910 sound index");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, drop, sizeof(drop));
    expect_err(rs_seq_decode(910, &buf, &s), RS_OK, "910 seq 23");
    rs_seq_free(&s);
    rs_buf_wrap(&buf, z, 1);
    expect_err(rs_seq_decode(474, &buf, &s), RS_ERR_UNSUPPORTED, "474 seq");
    expect_err(rs_seq_decode(530, &buf, &s), RS_ERR_UNSUPPORTED, "530 seq");
    expect_err(rs_seq_decode(831, &buf, &s), RS_ERR_UNSUPPORTED, "831 seq");
    expect_err(rs_seq_decode(317, NULL, &s), RS_ERR_ARG, "seq null");
}

static void test_spot_idk(void) {
    uint8_t spot[] = {1, 0, 5, 2, 0, 6, 7, 9, 40, 0xff, 0xff, 50, 0, 0x22, 0};
    uint8_t late[] = {9, 11, 40, 1, 0xff, 0xff, 0, 1, 0};
    uint8_t s910[] = {9, 46, 0};
    uint8_t miss[] = {11, 0};
    uint8_t idk[] = {1, 3, 3, 2, 1, 0, 9, 60, 0, 4, 40, 0, 2, 0};
    uint8_t idk2[] = {1, 9, 3, 2, 1, 0, 8, 60, 0, 1, 0};
    uint8_t z[] = {0};
    rs_buf buf;
    rs_spot_type sp;
    rs_identkit id;
    rs_buf_wrap(&buf, spot, sizeof(spot));
    expect_err(rs_spot_type_decode(317, &buf, &sp), RS_OK, "317 spot");
    expect(sp.model == 5 && sp.anim == 6 && sp.ambient == 9 && sp.resizeh == 128, "317 spot fields");
    expect(sp.recol_count == 6 && sp.recol_s[0] == 65535 && sp.recol_d[0] == 0x22, "317 recol raw");
    rs_spot_type_free(&sp);
    rs_buf_wrap(&buf, late, sizeof(late));
    expect_err(rs_spot_type_decode(742, &buf, &sp), RS_OK, "742 spot");
    expect(sp.hillskew_mode == 1 && sp.hillskew_value == 8224, "742 hillskew");
    expect(sp.recol_count == 1 && sp.recol_s[0] == -1 && sp.recol_d[0] == 1, "742 recol short");
    rs_spot_type_free(&sp);
    rs_buf_wrap(&buf, s910, sizeof(s910));
    expect_err(rs_spot_type_decode(910, &buf, &sp), RS_OK, "910 spot");
    expect(sp.hillskew_mode == 3 && sp.hillskew_value == 8224, "910 spot 46");
    rs_spot_type_free(&sp);
    rs_buf_wrap(&buf, miss, sizeof(miss));
    expect_err(rs_spot_type_decode(910, &buf, &sp), RS_ERR_FORMAT, "910 spot 11");
    rs_buf_wrap(&buf, idk, sizeof(idk));
    expect_err(rs_identkit_decode(317, &buf, &id), RS_OK, "317 idk");
    expect(id.type == 3 && id.disable == 1 && id.model_count == 1 && id.models[0] == 9, "317 idk model");
    expect(id.heads[0] == 4 && id.recol_s[0] == 2, "317 idk head");
    rs_identkit_free(&id);
    rs_buf_wrap(&buf, idk2, sizeof(idk2));
    expect_err(rs_identkit_decode(742, &buf, &id), RS_OK, "742 idk");
    expect(id.type == -1 && id.disable == 0 && id.models[0] == 8 && id.heads[0] == 1, "742 idk fields");
    rs_identkit_free(&id);
    rs_buf_wrap(&buf, z, 1);
    expect_err(rs_spot_type_decode(474, &buf, &sp), RS_ERR_UNSUPPORTED, "474 spot");
    expect_err(rs_identkit_decode(530, &buf, &id), RS_ERR_UNSUPPORTED, "530 idk");
    expect_err(rs_spot_type_decode(860, &buf, &sp), RS_ERR_UNSUPPORTED, "860 spot");
    expect_err(rs_identkit_decode(831, NULL, &id), RS_ERR_ARG, "idk null");
}

static void test_sprite(void) {
    uint8_t row[] = {0, 2, 0, 2, 2, 0, 0, 0, 1, 2, 0, 2, 0, 1, 0, 1, 1};
    uint8_t other[] = {0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 2, 0xab};
    uint8_t bad[] = {0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 5};
    uint8_t pal[] = {
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01
    };
    uint8_t drop[] = {
        0x02, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01
    };
    uint8_t col[] = {
        0x01, 10, 20, 30, 40, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x01
    };
    uint8_t full[] = {0, 0, 0, 1, 0, 1, 0x01, 0x02, 0x03, 0xff, 0xff};
    uint8_t mag[] = {0, 0, 0, 1, 0, 1, 0xff, 0x00, 0xff, 0xff, 0xff};
    uint8_t jpg[] = {1, 0, 0xff, 0xff};
    uint8_t f910[] = {0, 0, 0, 1, 0, 1, 0xff, 0x00, 0xff, 0x80, 0x01};
    uint8_t a910[] = {0, 1, 0, 1, 0, 1, 0xff, 0x00, 0xff, 0xaa, 0x80, 0x01};
    uint8_t j910[] = {1, 0, 0x80, 0x01};
    rs_buf buf;
    rs_sprite sp;
    rs_buf_wrap(&buf, row, sizeof(row));
    expect_err(rs_sprite_decode(317, &buf, &sp), RS_OK, "317 sprite");
    expect(sp.count == 1 && sp.palette_count == 2 && sp.palette[0] == 0 && sp.palette[1] == 1, "317 palette");
    expect(sp.frames[0].width == 2 && sp.frames[0].height == 1 && sp.frames[0].offset_x == 1, "317 crop");
    expect(sp.frames[0].pixels[0] == 1 && sp.frames[0].pixels[1] == 1 && sp.frames[0].flags == 0, "317 pixels");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, other, sizeof(other));
    expect_err(rs_sprite_decode(317, &buf, &sp), RS_OK, "317 sprite order");
    expect(sp.frames[0].flags == 2 && sp.frames[0].pixels[0] == 0, "317 unread pixels");
    expect(buf.pos + 1 == sizeof(other) && buf.data[buf.pos] == 0xab, "317 order stops");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, bad, sizeof(bad));
    expect_err(rs_sprite_decode(317, &buf, &sp), RS_ERR_FORMAT, "317 palette index");
    rs_buf_wrap(&buf, pal, sizeof(pal));
    expect_err(rs_sprite_decode(742, &buf, &sp), RS_OK, "742 paletted");
    expect(sp.full == 0 && sp.count == 1 && sp.palette[1] == 1 && sp.frames[0].indices[0] == 1, "742 index");
    expect(sp.frames[0].width == 1 && sp.frames[0].crop_x == 0 && sp.frames[0].alpha == NULL, "742 frame");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, pal, sizeof(pal));
    expect_err(rs_sprite_decode(910, &buf, &sp), RS_OK, "910 paletted");
    expect(sp.count == 1 && sp.frames[0].indices[0] == 1, "910 index");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, drop, sizeof(drop));
    expect_err(rs_sprite_decode(742, &buf, &sp), RS_OK, "742 alpha drop");
    expect(sp.frames[0].flags == 2 && sp.frames[0].alpha == NULL && sp.frames[0].indices[0] == 1, "742 alpha null");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, col, sizeof(col));
    expect_err(rs_sprite_decode(742, &buf, &sp), RS_OK, "742 column");
    expect(sp.frames[0].indices[0] == 10 && sp.frames[0].indices[1] == 30, "742 column x");
    expect(sp.frames[0].indices[2] == 20 && sp.frames[0].indices[3] == 40, "742 column y");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, full, sizeof(full));
    expect_err(rs_sprite_decode(742, &buf, &sp), RS_OK, "742 full");
    expect(sp.full == 1 && sp.count == 1 && (uint32_t)sp.frames[0].pixels[0] == 0xff010203u, "742 argb");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, mag, sizeof(mag));
    expect_err(rs_sprite_decode(742, &buf, &sp), RS_OK, "742 magenta");
    expect((uint32_t)sp.frames[0].pixels[0] == 0xffff00ffu, "742 keeps magenta");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, jpg, sizeof(jpg));
    expect_err(rs_sprite_decode(742, &buf, &sp), RS_ERR_UNSUPPORTED, "742 jpeg");
    rs_buf_wrap(&buf, f910, sizeof(f910));
    expect_err(rs_sprite_decode(910, &buf, &sp), RS_OK, "910 full");
    expect(sp.full == 1 && sp.count == 1 && sp.frames[0].pixels[0] == 0, "910 magenta zero");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, a910, sizeof(a910));
    expect_err(rs_sprite_decode(910, &buf, &sp), RS_OK, "910 alpha");
    expect((uint32_t)sp.frames[0].pixels[0] == 0xaa000000u, "910 alpha after magenta");
    rs_sprite_free(&sp);
    rs_buf_wrap(&buf, j910, sizeof(j910));
    expect_err(rs_sprite_decode(910, &buf, &sp), RS_ERR_UNSUPPORTED, "910 jpeg");
    expect_err(rs_sprite_decode(474, &buf, &sp), RS_ERR_UNSUPPORTED, "474 sprite");
    expect_err(rs_sprite_decode(317, NULL, &sp), RS_ERR_ARG, "sprite null");
}

static void glyph_hdr(tb *t, int w, int h, int fmt) {
    u1(t, 0);
    u1(t, 0);
    u2(t, w);
    u2(t, h);
    u1(t, fmt);
}

static void test_font(void) {
    tb t;
    uint8_t thin[12];
    uint8_t plain[263];
    uint8_t *kern;
    uint8_t *k910;
    uint8_t *bad;
    int8_t col[4] = {1, 2, 3, 4};
    int i;
    rs_buf buf;
    rs_font f;
    memset(&t, 0, sizeof(t));
    raw_put(&t, "\0\0\0\0", 4);
    u1(&t, 0);
    for (i = 0; i < 256; i++) {
        if (i == 0) {
            glyph_hdr(&t, 1, 1, 2);
        } else if (i == 10) {
            glyph_hdr(&t, 2, 7, 0);
            raw_put(&t, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 14);
        } else if (i == 11) {
            glyph_hdr(&t, 2, 2, 1);
            raw_put(&t, col, 4);
        } else if (i == 105) {
            glyph_hdr(&t, 4, 0, 0);
        } else {
            glyph_hdr(&t, 0, 0, 0);
        }
    }
    rs_buf_wrap(&buf, t.d, t.n);
    expect_err(rs_font_decode(317, &buf, &f), RS_OK, "317 font");
    expect(f.has_glyphs == 1 && f.height == 7, "317 font height");
    expect(f.glyph[105].advance == 4 && f.glyph[32].advance == 4 && f.space_advance == 4, "317 space");
    expect(f.glyph[0].advance == 1 && f.glyph[0].offset_x == 0 && f.glyph[0].pixels[0] == 0, "317 format other");
    expect(f.glyph[10].advance == 2 && f.glyph[10].offset_x == 0, "317 trim");
    expect(f.glyph[11].pixels[0] == 1 && f.glyph[11].pixels[1] == 3, "317 column 0");
    expect(f.glyph[11].pixels[2] == 2 && f.glyph[11].pixels[3] == 4 && f.glyph[11].advance == 4, "317 column 1");
    rs_font_free(&f);
    tb_free(&t);
    memset(thin, 0, sizeof(thin));
    thin[10] = 1;
    rs_buf_wrap(&buf, thin, sizeof(thin));
    expect_err(rs_font_decode(317, &buf, &f), RS_ERR_FORMAT, "317 font empty mask");
    memset(plain, 0, sizeof(plain));
    plain[2 + 65] = 5;
    plain[258] = 4;
    plain[259] = 1;
    plain[260] = 2;
    plain[261] = 8;
    plain[262] = 9;
    rs_buf_wrap(&buf, plain, sizeof(plain));
    expect_err(rs_font_decode(742, &buf, &f), RS_OK, "742 font");
    expect(f.has_metrics == 1 && f.kerning == NULL && f.advance[65] == 5, "742 advance");
    expect(f.space_advance == 4 && f.ascent == 8 && f.descent == 9, "742 metrics");
    rs_font_free(&f);
    kern = calloc(1, 774);
    kern[1] = 1;
    kern[2 + 65] = 5;
    kern[2 + 66] = 7;
    kern[772] = 3;
    kern[773] = 4;
    rs_buf_wrap(&buf, kern, 774);
    expect_err(rs_font_decode(742, &buf, &f), RS_OK, "742 kern");
    expect(f.kerning && f.kerning[65 * 256 + 66] == -5 && f.space_advance == 0, "742 kern cell");
    expect(f.kerning[32 * 256 + 66] == 0 && f.ascent == 3 && f.descent == 4, "742 kern skip");
    rs_font_free(&f);
    free(kern);
    k910 = calloc(1, 1803);
    k910[1] = 1;
    k910[2 + 65] = 5;
    k910[2 + 66] = 5;
    k910[1802] = 2;
    rs_buf_wrap(&buf, k910, 1803);
    expect_err(rs_font_decode(910, &buf, &f), RS_OK, "910 kern");
    expect(f.scale == 2 && f.top[65] == 2 && f.box[65][2] == 5, "910 scale");
    expect(f.kerning[65 * 256 + 66] == -2 && f.has_metrics == 1, "910 kern cell");
    rs_font_free(&f);
    free(k910);
    bad = calloc(1, 1804);
    bad[1798] = 9;
    bad[1803] = 0;
    rs_buf_wrap(&buf, bad, 1804);
    expect_err(rs_font_decode(910, &buf, &f), RS_ERR_FORMAT, "910 tint6 0");
    bad[1803] = 1;
    rs_buf_wrap(&buf, bad, 1804);
    expect_err(rs_font_decode(910, &buf, &f), RS_OK, "910 font");
    expect(f.space_advance == 9 && f.scale == 1 && f.kerning == NULL, "910 space");
    rs_font_free(&f);
    free(bad);
    rs_buf_wrap(&buf, (uint8_t *)"\1", 1);
    expect_err(rs_font_decode(742, &buf, &f), RS_ERR_FORMAT, "742 font version");
    expect_err(rs_font_decode(530, &buf, &f), RS_ERR_UNSUPPORTED, "530 font");
    expect_err(rs_font_decode(910, NULL, &f), RS_ERR_ARG, "font null");
}

static void test_script(void) {
    tb t;
    rs_buf buf;
    rs_clientscript cs;
    uint8_t z[] = {0};
    memset(&t, 0, sizeof(t));
    raw_put(&t, "cs", 2);
    u1(&t, 0);
    u2(&t, 2);
    u1(&t, 7);
    u2(&t, 145);
    raw_put(&t, "hi", 2);
    u1(&t, 0);
    u2(&t, 63);
    u8(&t, 42);
    u2(&t, 606);
    u4(&t, 43);
    script_seal(&t, 4);
    rs_buf_wrap(&buf, t.d, t.n);
    expect_err(rs_clientscript_decode(742, &buf, &cs), RS_OK, "742 script");
    expect(cs.name_set && strcmp(cs.name, "cs") == 0 && cs.op_count == 4, "742 script name");
    expect(cs.ops[0].opcode == 2 && cs.ops[0].int_set && cs.ops[0].int_operand == 7, "742 op2");
    expect(cs.ops[1].opcode == 145 && strcmp(cs.ops[1].str_operand, "hi") == 0, "742 string");
    expect(cs.ops[2].opcode == 63 && cs.ops[2].long_set && cs.ops[2].long_operand == 42, "742 long");
    expect(cs.ops[3].opcode == 606 && cs.ops[3].int_operand == 43, "742 wide");
    rs_clientscript_free(&cs);
    tb_free(&t);
    memset(&t, 0, sizeof(t));
    u1(&t, 0);
    u2(&t, 1046);
    u1(&t, 1);
    script_seal(&t, 1);
    rs_buf_wrap(&buf, t.d, t.n);
    expect_err(rs_clientscript_decode(742, &buf, &cs), RS_ERR_FORMAT, "742 opcode range");
    tb_free(&t);
    memset(&t, 0, sizeof(t));
    u1(&t, 0);
    u4(&t, 0);
    {
        int i;
        for (i = 0; i < 6; i++) {
            u2(&t, 0);
        }
    }
    u1(&t, 1);
    u2(&t, 1);
    u4(&t, 4);
    u4(&t, 9);
    u2(&t, 11);
    rs_buf_wrap(&buf, t.d, t.n);
    expect_err(rs_clientscript_decode(742, &buf, &cs), RS_OK, "742 switch");
    expect(cs.name == NULL && cs.name_set == 0 && cs.op_count == 0, "742 empty name");
    expect(cs.switch_count == 1 && cs.switches[0].count == 1, "742 switch count");
    expect(cs.switches[0].cases[0].key == 4 && cs.switches[0].cases[0].value == 9, "742 case");
    rs_clientscript_free(&cs);
    tb_free(&t);
    memset(&t, 0, sizeof(t));
    raw_put(&t, "q", 1);
    u1(&t, 0);
    u2(&t, 1376);
    u1(&t, 2);
    raw_put(&t, "z", 1);
    u1(&t, 0);
    u2(&t, 1376);
    u1(&t, 0);
    u4(&t, 9);
    u2(&t, 1376);
    u1(&t, 1);
    u8(&t, 1);
    u2(&t, 524);
    u1(&t, 0);
    u2(&t, 4);
    u1(&t, 3);
    u2(&t, 524);
    u1(&t, 99);
    u2(&t, 1);
    u1(&t, 2);
    u2(&t, 1351);
    u2(&t, 8);
    u1(&t, 6);
    script_seal(&t, 6);
    rs_buf_wrap(&buf, t.d, t.n);
    expect_err(rs_clientscript_decode(910, &buf, &cs), RS_OK, "910 script");
    expect(cs.ops[0].opcode == 1376 && strcmp(cs.ops[0].str_operand, "z") == 0, "910 string");
    expect(cs.ops[1].opcode == 204 && cs.ops[1].int_operand == 9, "910 int rewrite");
    expect(cs.ops[2].opcode == 761 && cs.ops[2].long_set && cs.ops[2].long_operand == 1, "910 long rewrite");
    expect(cs.ops[3].opcode == 524 && cs.ops[3].domain == 0 && cs.ops[3].var_id == 4, "910 push var");
    expect(cs.ops[3].int_operand == 3, "910 var extra");
    expect(cs.ops[4].domain == 99 && cs.ops[4].var_id == 1 && cs.ops[4].int_operand == 2, "910 domain");
    expect(cs.ops[5].opcode == 1351 && cs.ops[5].var_id == 8 && cs.ops[5].int_operand == 6, "910 varbit");
    expect(cs.ops[5].domain == -1, "910 varbit domain");
    rs_clientscript_free(&cs);
    tb_free(&t);
    memset(&t, 0, sizeof(t));
    raw_put(&t, "n", 1);
    u1(&t, 0);
    u2(&t, 1376);
    u1(&t, 3);
    script_seal(&t, 1);
    rs_buf_wrap(&buf, t.d, t.n);
    expect_err(rs_clientscript_decode(910, &buf, &cs), RS_ERR_FORMAT, "910 string type");
    tb_free(&t);
    rs_buf_wrap(&buf, z, 1);
    expect_err(rs_clientscript_decode(317, &buf, &cs), RS_ERR_UNSUPPORTED, "317 script");
    expect_err(rs_clientscript_decode(474, &buf, &cs), RS_ERR_UNSUPPORTED, "474 script");
    expect_err(rs_clientscript_decode(530, &buf, &cs), RS_ERR_UNSUPPORTED, "530 script");
    expect_err(rs_clientscript_decode(831, &buf, &cs), RS_ERR_UNSUPPORTED, "831 script");
    expect_err(rs_clientscript_decode(860, &buf, &cs), RS_ERR_UNSUPPORTED, "860 script");
    expect_err(rs_clientscript_decode(742, NULL, &cs), RS_ERR_ARG, "script null");
}

static int32_t jag_hash(const char *s) {
    uint32_t h = 0;
    while (*s) {
        h = h * 61u + (uint32_t)(unsigned char)(*s) - 32u;
        s++;
    }
    return (int32_t)h;
}

static int cache_file(const char *dir, int file_id, uint8_t **out, size_t *out_n) {
    char idx_path[512];
    char dat_path[512];
    FILE *idx;
    FILE *dat;
    uint8_t ent[6];
    uint8_t sec[520];
    unsigned len;
    unsigned sector;
    unsigned chunk;
    unsigned got;
    uint8_t *buf;
    snprintf(idx_path, sizeof(idx_path), "%s/main_file_cache.idx0", dir);
    snprintf(dat_path, sizeof(dat_path), "%s/main_file_cache.dat", dir);
    idx = fopen(idx_path, "rb");
    dat = fopen(dat_path, "rb");
    if (!idx || !dat) {
        if (idx) {
            fclose(idx);
        }
        if (dat) {
            fclose(dat);
        }
        return 1;
    }
    if (fseek(idx, file_id * 6L, SEEK_SET) != 0 || fread(ent, 1, 6, idx) != 6) {
        fclose(idx);
        fclose(dat);
        return -1;
    }
    fclose(idx);
    len = ((unsigned)ent[0] << 16) | ((unsigned)ent[1] << 8) | ent[2];
    sector = ((unsigned)ent[3] << 16) | ((unsigned)ent[4] << 8) | ent[5];
    if (len == 0 || len > 500000) {
        fclose(dat);
        return -1;
    }
    buf = malloc(len);
    if (!buf) {
        fclose(dat);
        return -1;
    }
    got = 0;
    chunk = 0;
    while (got < len) {
        unsigned n = len - got;
        unsigned fid;
        unsigned part;
        unsigned next;
        unsigned arch;
        if (n > 512) {
            n = 512;
        }
        if (sector == 0 || fseek(dat, (long)sector * 520L, SEEK_SET) != 0 || fread(sec, 1, 8u + n, dat) != 8u + n) {
            free(buf);
            fclose(dat);
            return -1;
        }
        fid = ((unsigned)sec[0] << 8) | sec[1];
        part = ((unsigned)sec[2] << 8) | sec[3];
        next = ((unsigned)sec[4] << 16) | ((unsigned)sec[5] << 8) | sec[6];
        arch = sec[7];
        if (fid != (unsigned)file_id || part != chunk || arch != 1) {
            free(buf);
            fclose(dat);
            return -1;
        }
        memcpy(buf + got, sec + 8, n);
        got += n;
        sector = next;
        chunk++;
    }
    fclose(dat);
    *out = buf;
    *out_n = len;
    return 0;
}

static int jag_has(const uint8_t *d, size_t n, int32_t hash) {
    unsigned count;
    size_t pos;
    unsigned i;
    if (n < 8) {
        return 0;
    }
    count = ((unsigned)d[6] << 8) | d[7];
    pos = 8;
    for (i = 0; i < count; i++) {
        int32_t got;
        if (pos + 10 > n) {
            return 0;
        }
        got = (int32_t)(((uint32_t)d[pos] << 24) | ((uint32_t)d[pos + 1] << 16) | ((uint32_t)d[pos + 2] << 8) | d[pos + 3]);
        if (got == hash) {
            return 1;
        }
        pos += 10;
    }
    return 0;
}

static void test_config_archive(void) {
    const char *roots[] = {
        "builds/rs2/317-2005-06-13/cache",
        NULL
    };
    int i;
    int found_root = 0;
    expect(jag_hash("VARBIT.DAT") == (int32_t)-514869585, "varbit hash");
    for (i = 0; roots[i]; i++) {
        uint8_t *data = NULL;
        size_t n = 0;
        unsigned unpacked;
        unsigned packed;
        int rc = cache_file(roots[i], 2, &data, &n);
        if (rc == 1) {
            continue;
        }
        found_root = 1;
        expect(rc == 0 && data && n >= 6, "config archive");
        if (rc != 0 || !data || n < 6) {
            free(data);
            return;
        }
        unpacked = ((unsigned)data[0] << 16) | ((unsigned)data[1] << 8) | data[2];
        packed = ((unsigned)data[3] << 16) | ((unsigned)data[4] << 8) | data[5];
        if (unpacked == packed) {
            expect(jag_has(data, n, jag_hash("VARBIT.DAT")), "config varbit.dat");
            expect(jag_has(data, n, jag_hash("SEQ.DAT")), "config seq.dat");
            expect(jag_has(data, n, jag_hash("SPOTANIM.DAT")), "config spotanim.dat");
            expect(jag_has(data, n, jag_hash("IDK.DAT")), "config idk.dat");
        }
        free(data);
        return;
    }
    (void)found_root;
}

int main(void) {
    test_varbit();
    test_enum();
    test_seq();
    test_spot_idk();
    test_sprite();
    test_font();
    test_script();
    test_config_archive();
    return g_failed ? 1 : 0;
}
