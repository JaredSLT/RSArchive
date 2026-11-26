#include "rslib/both/content.h"

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

static void test_obj_name(void) {
    uint8_t raw[] = {2, 'n', 'a', 'm', 'e', 10, 0};
    rs_buf buf;
    rs_obj_type obj;
    rs_buf_wrap(&buf, raw, sizeof(raw));
    expect_err(rs_obj_type_decode(317, &buf, &obj), RS_OK, "317 obj");
    expect(obj.name_set && strcmp(obj.name, "name") == 0, "317 obj name");
    expect(obj.zoom2d == 2000 && obj.cost == 1 && obj.manwear == -1 && obj.dummy10 == -1, "317 obj defaults");
    expect(obj.id == 0, "317 obj id");

    raw[0] = 99;
    rs_buf_wrap(&buf, raw, 1);
    expect_err(rs_obj_type_decode(317, &buf, &obj), RS_ERR_FORMAT, "317 obj unknown");
    expect_err(rs_obj_type_decode(831, &buf, &obj), RS_ERR_UNSUPPORTED, "obj 831");
    expect_err(rs_obj_type_decode(860, &buf, &obj), RS_ERR_UNSUPPORTED, "obj 860");
    expect_err(rs_obj_type_decode(317, NULL, &obj), RS_ERR_ARG, "obj null");
}

static void test_obj_contrast(void) {
    uint8_t raw[] = {114, 2, 0};
    rs_buf buf;
    rs_obj_type obj;
    rs_buf_wrap(&buf, raw, sizeof(raw));
    expect_err(rs_obj_type_decode(317, &buf, &obj), RS_OK, "317 contrast");
    expect(obj.contrast == 10, "317 contrast *5");
    rs_buf_wrap(&buf, raw, sizeof(raw));
    expect_err(rs_obj_type_decode(742, &buf, &obj), RS_OK, "742 contrast");
    expect(obj.contrast == 2 && strcmp(obj.name, "null") == 0 && obj.zoom2d == 2000, "742 contrast raw");
    rs_buf_wrap(&buf, raw, sizeof(raw));
    expect_err(rs_obj_type_decode(910, &buf, &obj), RS_OK, "910 contrast");
    expect(obj.contrast == 10 && obj.tradeable == 1 && obj.placeholder == 1, "910 contrast *5");
    raw[0] = 0;
    rs_buf_wrap(&buf, raw, 1);
    expect_err(rs_obj_type_decode(530, &buf, &obj), RS_OK, "530 empty");
    expect(obj.zoom2d == 0 && obj.cost == 1 && obj.womanwear2 == -1 && obj.certtemplate == 0, "530 defaults");
    expect(obj.certlink == -1 && obj.op127_a == -1 && obj.name_set == 0, "530 more defaults");
}

static void test_loc_finish(void) {
    uint8_t named[] = {2, 'd', 'o', 'o', 'r', 10, 0};
    uint8_t br[] = {74, 0};
    uint8_t c39[] = {39, 2, 0};
    rs_buf buf;
    rs_loc_type loc;
    rs_buf_wrap(&buf, named, sizeof(named));
    expect_err(rs_loc_type_decode(317, &buf, &loc), RS_OK, "317 loc");
    expect(loc.name_set && strcmp(loc.name, "door") == 0, "317 loc name");
    expect(loc.width == 1 && loc.length == 1 && loc.blockwalk == 1 && loc.blockrange == 1, "317 loc size");
    expect(loc.active == 0 && loc.raiseobject == 1 && loc.walloff == 16, "317 loc finish");

    rs_buf_wrap(&buf, br, sizeof(br));
    expect_err(rs_loc_type_decode(317, &buf, &loc), RS_OK, "317 breakroute");
    expect(loc.blockwalk == 0 && loc.blockrange == 0 && loc.raiseobject == 0, "317 break then raise");

    rs_buf_wrap(&buf, br, sizeof(br));
    expect_err(rs_loc_type_decode(474, &buf, &loc), RS_OK, "474 breakroute");
    expect(loc.blockwalk == 0 && loc.blockrange == 0 && loc.raiseobject == 1, "474 raise then break");

    rs_buf_wrap(&buf, c39, sizeof(c39));
    expect_err(rs_loc_type_decode(317, &buf, &loc), RS_OK, "317 loc contrast");
    expect(loc.contrast == 2, "317 loc contrast raw");
    rs_buf_wrap(&buf, c39, sizeof(c39));
    expect_err(rs_loc_type_decode(474, &buf, &loc), RS_OK, "474 loc contrast");
    expect(loc.contrast == 10, "474 loc contrast *5");
    rs_buf_wrap(&buf, c39, sizeof(c39));
    expect_err(rs_loc_type_decode(742, &buf, &loc), RS_OK, "742 loc contrast");
    expect(loc.contrast == 2, "742 loc contrast raw");
    rs_buf_wrap(&buf, c39, sizeof(c39));
    expect_err(rs_loc_type_decode(910, &buf, &loc), RS_OK, "910 loc contrast");
    expect(loc.contrast == 10 && loc.blockrange == 1, "910 loc contrast *5");
}

static void test_npc_size(void) {
    uint8_t raw[] = {12, 0xff, 0};
    rs_buf buf;
    rs_npc_type npc;
    rs_buf_wrap(&buf, raw, sizeof(raw));
    expect_err(rs_npc_type_decode(317, &buf, &npc), RS_OK, "317 npc");
    expect(npc.size == -1 && npc.turnspeed == 32 && npc.readyanim == -1, "317 npc signed size");
    rs_buf_wrap(&buf, raw, sizeof(raw));
    expect_err(rs_npc_type_decode(474, &buf, &npc), RS_OK, "474 npc");
    expect(npc.size == 255 && npc.walksmoothing == 1, "474 npc unsigned size");
    {
        uint8_t op35[] = {35, 'x', 10};
        rs_buf_wrap(&buf, op35, sizeof(op35));
        expect_err(rs_npc_type_decode(317, &buf, &npc), RS_ERR_RANGE, "317 npc op range");
    }
}

static void test_land(void) {
    enum { tiles = 4 * 64 * 64 };
    uint8_t *raw;
    rs_land *land;
    size_t i;
    raw = (uint8_t *)malloc(3 + (size_t)(tiles - 1));
    land = (rs_land *)malloc(sizeof(*land));
    expect(raw && land, "land alloc");
    if (!raw || !land) {
        free(raw);
        free(land);
        return;
    }
    raw[0] = 50;
    raw[1] = 1;
    raw[2] = 2;
    for (i = 3; i < 3 + (size_t)(tiles - 1); i++) {
        raw[i] = 0;
    }
    expect_err(rs_land_decode(317, raw, 3 + (size_t)(tiles - 1), land), RS_OK, "land");
    expect(land->height[0][0][0] == -16, "land height");
    {
        int perlin_origin = land->height[0][0][1];
        expect_err(rs_land_decode_at(317, raw, 3 + (size_t)(tiles - 1), 3200, 3200, land), RS_OK,
                   "land origin");
        expect(land->height[0][0][0] == -16, "opcode 1 ignores origin");
        expect(land->height[0][0][1] != perlin_origin, "opcode 0 uses origin");
        expect_err(rs_land_decode(317, raw, 3 + (size_t)(tiles - 1), land), RS_OK, "land restore");
    }
    expect(land->flags[0][0][0] == 1, "land flags");
    expect((land->collision[0][0][0] & 2097152) != 0, "land blocked");
    expect(land->underlay[0][0][0] == 0 && land->overlay[0][0][0] == 0, "land unset layers");
    expect_err(rs_land_decode(254, raw, 3 + (size_t)(tiles - 1), land), RS_OK, "land 254");
    expect(land->height[0][0][0] == -16 && (land->collision[0][0][0] & 2097152) != 0, "land 254 scale");
    expect_err(rs_land_decode(578, raw, 3 + (size_t)(tiles - 1), land), RS_OK, "land 578");
    expect(land->height[0][0][0] == -16, "land 578 scale 8");
    expect_err(rs_land_decode(614, raw, 3 + (size_t)(tiles - 1), land), RS_OK, "land 614");
    expect(land->height[0][0][0] == -64 && land->flags[0][0][0] == 1, "land 614 scale 32");
    expect((land->collision[0][0][0] & 2097152) != 0, "land 614 blocked");
    expect_err(rs_land_decode(742, raw, 3 + (size_t)(tiles - 1), land), RS_OK, "land 742");
    expect(land->height[0][0][0] == -64, "land 742 scale 32");
    expect_err(rs_land_decode(695, raw, 3 + (size_t)(tiles - 1), land), RS_ERR_UNSUPPORTED, "land 695");
    expect_err(rs_land_decode(831, raw, 3 + (size_t)(tiles - 1), land), RS_ERR_UNSUPPORTED, "land 831");
    expect_err(rs_land_decode(860, raw, 3 + (size_t)(tiles - 1), land), RS_ERR_UNSUPPORTED, "land 860");
    expect_err(rs_land_decode(115, raw, 3 + (size_t)(tiles - 1), land), RS_ERR_UNSUPPORTED, "land 115");
    expect_err(rs_land_decode(317, raw, 1, land), RS_ERR_FORMAT, "land short");
    expect_err(rs_land_decode(317, NULL, 1, land), RS_ERR_ARG, "land null");
    free(raw);
    free(land);
    {
        uint8_t *bits;
        size_t n = 3 + (size_t)(tiles - 1);
        bits = (uint8_t *)malloc(n);
        land = (rs_land *)malloc(sizeof(*land));
        expect(bits && land, "land bits alloc");
        if (!bits || !land) {
            free(bits);
            free(land);
            return;
        }
        memset(bits, 0, n);
        bits[0] = 0x0a;
        bits[1] = 1;
        bits[2] = 2;
        expect_err(rs_land_decode(745, bits, n, land), RS_OK, "land 745");
        expect(land->height[0][0][0] == -64 && land->flags[0][0][0] == 1, "land 745 height");
        expect((land->collision[0][0][0] & 2097152) != 0, "land 745 blocked");
        expect_err(rs_land_decode(910, bits, n, land), RS_OK, "land 910");
        expect(land->height[0][0][0] == -64 && land->flags[0][0][0] == 1, "land 910 height");
        expect((land->collision[0][0][0] & 2097152) == 0, "land 910 no stamp");
        expect_err(rs_land_decode(790, bits, 1, land), RS_ERR_FORMAT, "land 790 short");
        free(bits);
        free(land);
    }
}

static void test_flo(void) {
    uint8_t raw[] = {0, 1, 1, 0, 0, 255, 0};
    rs_flo_type flo;
    int count = -1;

    memset(&flo, 0, sizeof flo);
    expect_err(rs_flo_decode(317, raw, sizeof raw, &flo, 1, &count), RS_OK, "flo");
    expect(count == 1, "flo count");
    expect(flo.rgb == 255 && flo.texture == -1 && flo.occlude == 1 && flo.overlay == 0, "flo fields");
    expect(flo.hue == 170 && flo.saturation == 255 && flo.lightness == 127, "flo hsl channels");
    expect(flo.luminance == 255 && flo.chroma == 170 && flo.hsl == 43967, "flo pack");
    expect_err(rs_flo_decode(254, raw, sizeof raw, &flo, 1, &count), RS_OK, "flo 254");
    expect(flo.hue == 170 && flo.hsl == 43967, "flo 254 colour");
    expect_err(rs_flo_decode(282, raw, sizeof raw, &flo, 1, &count), RS_OK, "flo 282");
    expect_err(rs_flo_decode(350, raw, sizeof raw, &flo, 1, &count), RS_OK, "flo 350");
    expect_err(rs_flo_decode(377, raw, sizeof raw, &flo, 1, &count), RS_OK, "flo 377");
    {
        uint8_t op7[] = {0, 1, 7, 0, 0, 255, 0};
        expect_err(rs_flo_decode(254, op7, sizeof op7, &flo, 1, &count), RS_ERR_FORMAT, "flo 254 op7");
        expect_err(rs_flo_decode(317, op7, sizeof op7, &flo, 1, &count), RS_OK, "flo 317 op7");
        expect(flo.rgb == 255 && flo.hue == 0 && flo.luminance == 0, "flo op7 keeps channels");
    }
    expect_err(rs_flo_decode(474, raw, sizeof raw, &flo, 1, &count), RS_ERR_UNSUPPORTED, "flo 474");
    expect_err(rs_flo_decode(831, raw, sizeof raw, &flo, 1, &count), RS_ERR_UNSUPPORTED, "flo 831");
    expect_err(rs_flo_decode(317, raw, 1, &flo, 1, &count), RS_ERR_FORMAT, "flo short");
}

static void expect_js5(int rev, int kind, const uint8_t *raw, size_t n, int rgb, int tex, int occ, int lum,
                       const char *msg) {
    rs_flo_type flo;
    char line[96];

    memset(&flo, 0x5a, sizeof flo);
    snprintf(line, sizeof line, "%s decode", msg);
    expect_err(rs_flo_js5_decode(rev, kind, raw, n, &flo), RS_OK, line);
    snprintf(line, sizeof line, "%s fields", msg);
    expect(flo.rgb == rgb && flo.texture == tex && flo.occlude == occ && flo.luminance == lum, line);
}

static void test_flo_js5(void) {
    rs_flo_type flo;
    uint8_t blue[] = {1, 0, 0, 255, 0};
    uint8_t mag[] = {1, 0xff, 0x00, 0xff, 0};
    uint8_t u443[] = {1, 0, 0, 255, 2, 0, 5, 0};
    uint8_t u474[] = {1, 0, 0, 255, 2, 0, 5, 0};
    uint8_t u3[] = {3, 0, 2, 2, 0, 7, 0};
    uint8_t u5[] = {5, 1, 0, 0, 255, 0};
    uint8_t o443[] = {2, 1, 3, 0, 7, 0};
    uint8_t o474[] = {2, 1, 3, 0, 7, 0};
    uint8_t o11[] = {0x0b, 2, 2, 5, 0};
    uint8_t o15[] = {0x0f, 2, 7, 0};
    uint8_t o16[] = {0x10, 2, 2, 5, 0};
    uint8_t o20[] = {0x0e, 0x10, 0x14, 0x00, 0x07, 0x02, 0x04, 0};
    uint8_t sec[] = {1, 0, 0, 255, 7, 0xff, 0x00, 0xff, 0};
    uint8_t empty[] = {0};
    int sib[] = {634, 667, 742, 745, 790, 876, 887, 902, 910};
    int i;

    expect_err(rs_flo_js5_decode(474, RS_FLO_UNDERLAY, NULL, 1, &flo), RS_ERR_ARG, "js5 null");
    expect_err(rs_flo_js5_decode(474, RS_FLO_UNDERLAY, blue, 0, &flo), RS_ERR_FORMAT, "js5 empty");
    expect_err(rs_flo_js5_decode(474, 9, blue, sizeof blue, &flo), RS_ERR_ARG, "js5 kind");
    expect_err(rs_flo_js5_decode(695, RS_FLO_UNDERLAY, blue, sizeof blue, &flo), RS_ERR_UNSUPPORTED, "js5 695");
    expect_err(rs_flo_js5_decode(831, RS_FLO_OVERLAY, blue, sizeof blue, &flo), RS_ERR_UNSUPPORTED, "js5 831");
    expect_err(rs_flo_js5_decode(860, RS_FLO_OVERLAY, blue, sizeof blue, &flo), RS_ERR_UNSUPPORTED, "js5 860");

    expect_js5(474, RS_FLO_UNDERLAY, blue, sizeof blue, 255, -1, 1, 255, "474 under blue");
    expect_err(rs_flo_js5_decode(474, RS_FLO_UNDERLAY, blue, sizeof blue, &flo), RS_OK, "474 under hsl");
    expect(flo.hue == 170 && flo.saturation == 255 && flo.lightness == 127 && flo.chroma == 170 && flo.hsl == 43967,
           "474 under channels");
    expect_js5(443, RS_FLO_UNDERLAY, u443, sizeof u443, 255, -1, 1, 255, "443 under ignores op2");
    expect_js5(468, RS_FLO_UNDERLAY, u443, sizeof u443, 255, -1, 1, 255, "468 under ignores op2");
    expect_js5(474, RS_FLO_UNDERLAY, u474, sizeof u474, 255, 5, 1, 255, "474 under tex");
    expect_js5(474, RS_FLO_UNDERLAY, u3, sizeof u3, 0, -1, 1, 1, "474 under op3 stops");
    expect_js5(481, RS_FLO_UNDERLAY, u3, sizeof u3, 0, 7, 1, 0, "481 under op3 width");
    expect_js5(530, RS_FLO_UNDERLAY, u3, sizeof u3, 0, 7, 1, 0, "530 under op3 width");
    expect_js5(578, RS_FLO_UNDERLAY, u3, sizeof u3, 0, 7, 1, 0, "578 under op3 width");
    expect_js5(614, RS_FLO_UNDERLAY, u3, sizeof u3, 0, 7, 1, 0, "614 under op3 width");
    expect_js5(614, RS_FLO_UNDERLAY, u5, sizeof u5, 255, -1, 1, 255, "614 under op5 stays");
    expect_js5(634, RS_FLO_UNDERLAY, u5, sizeof u5, 255, -1, 0, 255, "634 under op5");
    for (i = 0; i < 9; i++) {
        expect_js5(sib[i], RS_FLO_UNDERLAY, u5, sizeof u5, 255, -1, 0, 255, "under op5 family");
    }
    expect_js5(443, RS_FLO_UNDERLAY, empty, sizeof empty, 0, -1, 1, 1, "443 under post");
    expect_js5(910, RS_FLO_UNDERLAY, empty, sizeof empty, 0, -1, 1, 0, "910 under no post");
    expect_js5(474, RS_FLO_UNDERLAY, mag, sizeof mag, 16711935, -1, 1, 255, "474 under magenta");
    expect_err(rs_flo_js5_decode(474, RS_FLO_UNDERLAY, mag, sizeof mag, &flo), RS_OK, "magenta channels");
    expect(flo.hue == -42 && flo.saturation == 255 && flo.lightness == 127, "magenta hsl");

    expect_js5(443, RS_FLO_OVERLAY, o443, sizeof o443, 0, 1, 1, 1, "443 over op3 stops");
    expect_js5(468, RS_FLO_OVERLAY, o443, sizeof o443, 0, 1, 1, 1, "468 over op3 stops");
    expect_js5(474, RS_FLO_OVERLAY, o474, sizeof o474, 0, 7, 1, 1, "474 over op3 tex");
    expect_js5(474, RS_FLO_OVERLAY, mag, sizeof mag, 16711935, -1, 1, 255, "474 over magenta");
    expect_js5(474, RS_FLO_OVERLAY, sec, sizeof sec, 255, -1, 1, 255, "474 over secondary");
    expect_js5(481, RS_FLO_OVERLAY, o11, sizeof o11, 0, 2, 0, 0, "481 over op11");
    expect_js5(530, RS_FLO_OVERLAY, o11, sizeof o11, 0, 5, 1, 0, "530 over op11");
    expect_js5(508, RS_FLO_OVERLAY, o11, sizeof o11, 0, 5, 1, 0, "508 over op11");
    expect_js5(525, RS_FLO_OVERLAY, o11, sizeof o11, 0, 5, 1, 0, "525 over op11");
    expect_js5(614, RS_FLO_OVERLAY, o15, sizeof o15, 0, -1, 1, 0, "614 over op15");
    expect_js5(578, RS_FLO_OVERLAY, o15, sizeof o15, 0, -1, 1, 0, "578 over op15");
    expect_js5(634, RS_FLO_OVERLAY, o15, sizeof o15, 0, 7, 1, 0, "634 over op15");
    expect_js5(667, RS_FLO_OVERLAY, o15, sizeof o15, 0, 7, 1, 0, "667 over op15");
    expect_js5(530, RS_FLO_OVERLAY, o16, sizeof o16, 0, 2, 0, 0, "530 over op16");
    expect_js5(634, RS_FLO_OVERLAY, o16, sizeof o16, 0, 5, 1, 0, "634 over op16");
    expect_js5(634, RS_FLO_OVERLAY, o20, sizeof o20, 0, -1, 1, 0, "634 over op20 stops");
    expect_js5(742, RS_FLO_OVERLAY, o20, sizeof o20, 0, 4, 1, 0, "742 over op20");
    expect_js5(910, RS_FLO_OVERLAY, o20, sizeof o20, 0, 4, 1, 0, "910 over op20");
    expect_js5(745, RS_FLO_OVERLAY, o20, sizeof o20, 0, 4, 1, 0, "745 over op20");
    expect_js5(790, RS_FLO_OVERLAY, o20, sizeof o20, 0, 4, 1, 0, "790 over op20");
    expect_js5(876, RS_FLO_OVERLAY, o20, sizeof o20, 0, 4, 1, 0, "876 over op20");
    expect_js5(887, RS_FLO_OVERLAY, o20, sizeof o20, 0, 4, 1, 0, "887 over op20");
    expect_js5(902, RS_FLO_OVERLAY, o20, sizeof o20, 0, 4, 1, 0, "902 over op20");
    expect_err(rs_flo_js5_decode(481, RS_FLO_OVERLAY, blue, 2, &flo), RS_ERR_FORMAT, "js5 short colour");
}

static void test_locs(void) {
    uint8_t raw[] = {1, 66, 40, 0, 1, 2, 0, 0, 0};
    rs_land *land;
    rs_land_loc locs[4];
    rs_land_loc_type types[2];
    int count = -1;
    land = (rs_land *)malloc(sizeof(*land));
    expect(land != NULL, "locs land alloc");
    if (!land) {
        return;
    }
    memset(land, 0, sizeof(*land));
    memset(types, 0, sizeof(types));
    types[0].width = 1;
    types[0].length = 1;
    types[0].blockwalk = 1;
    types[0].blockrange = 1;
    types[0].active = 1;
    expect_err(rs_land_locs_decode(317, raw, sizeof(raw), land, types, 2, locs, 4, &count), RS_OK, "locs");
    expect(count == 2, "locs count");
    expect(locs[0].id == 0 && locs[0].x == 1 && locs[0].z == 1 && locs[0].level == 0, "locs first");
    expect(locs[0].shape == 10 && locs[0].rot == 0 && locs[0].plane == 0, "locs shape");
    expect((land->collision[0][1][1] & 131328) == 131328, "locs collision");
    expect(locs[1].id == 1 && locs[1].x == 0 && locs[1].z == 1 && locs[1].shape == 0, "locs accumulate");
    expect(land->collision[0][0][1] == 0, "locs edge skipped");
    raw[0] = 0;
    expect_err(rs_land_locs_decode(317, raw, 1, NULL, NULL, 0, locs, 4, &count), RS_OK, "locs empty");
    expect(count == 0, "locs empty count");
    expect_err(rs_land_locs_decode(474, raw, 1, NULL, NULL, 0, locs, 4, &count), RS_ERR_UNSUPPORTED, "locs 474");
    free(land);
}

static void test_collision_toggle(void) {
    rs_land *land;
    rs_land_loc_type ty;
    land = calloc(1, sizeof(*land));
    expect(land != NULL, "collision alloc");
    if (!land) {
        return;
    }
    memset(&ty, 0, sizeof ty);
    ty.width = 1;
    ty.length = 1;
    ty.blockwalk = 1;
    ty.blockrange = 1;
    expect_err(rs_land_collision_add(land, 317, 0, 10, 10, 0, 0, &ty), RS_OK, "add wall");
    expect((land->collision[0][10][10] & 128) != 0, "wall bit");
    expect((land->collision[0][9][10] & 8) != 0, "wall neighbour");
    expect((land->collision[0][10][10] & 65536) != 0, "wall range");
    expect_err(rs_land_collision_remove(land, 317, 0, 10, 10, 0, 0, &ty), RS_OK, "remove wall");
    expect(land->collision[0][10][10] == 0 && land->collision[0][9][10] == 0, "wall cleared");
    expect_err(rs_land_collision_add(land, 317, 0, 5, 5, 10, 0, &ty), RS_OK, "add centre");
    expect((land->collision[0][5][5] & 131328) == 131328, "centre bits");
    expect_err(rs_land_collision_remove(land, 317, 0, 5, 5, 10, 0, &ty), RS_OK, "remove centre");
    expect(land->collision[0][5][5] == 0, "centre cleared");
    ty.active = 1;
    expect_err(rs_land_collision_add(land, 317, 0, 4, 4, 22, 0, &ty), RS_OK, "add decor");
    expect((land->collision[0][4][4] & 2097152) != 0, "decor bit");
    expect_err(rs_land_collision_remove(land, 317, 0, 4, 4, 22, 0, &ty), RS_OK, "remove decor");
    expect(land->collision[0][4][4] == 0, "decor cleared");
    ty.blockwalk = 0;
    land->collision[0][5][5] = 7;
    expect_err(rs_land_collision_add(land, 317, 0, 5, 5, 10, 0, &ty), RS_OK, "no blockwalk");
    expect(land->collision[0][5][5] == 7, "no blockwalk unchanged");
    expect_err(rs_land_collision_add(land, 317, 0, 0, 5, 0, 0, &ty), RS_OK, "border");
    expect_err(rs_land_collision_add(land, 474, 0, 5, 5, 0, 0, &ty), RS_ERR_UNSUPPORTED, "collision 474");
    expect_err(rs_land_collision_add(NULL, 317, 0, 5, 5, 0, 0, &ty), RS_ERR_ARG, "collision null");
    expect_err(rs_land_collision_remove(land, 317, 4, 5, 5, 0, 0, &ty), RS_ERR_ARG, "collision plane");
    free(land);
}

int main(void) {
    test_obj_name();
    test_obj_contrast();
    test_loc_finish();
    test_npc_size();
    test_land();
    test_flo();
    test_flo_js5();
    test_locs();
    test_collision_toggle();
    if (g_failed) {
        fprintf(stderr, "test_content failed\n");
        return 1;
    }
    return 0;
}
