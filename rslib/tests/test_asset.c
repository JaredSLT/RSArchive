#include "rslib/both/asset.h"

#include <stdio.h>
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

/* 18 zero bytes: vertex count 0, face count 0, no optional sections. */
static void test_model_empty(void) {
    uint8_t raw[18];
    rs_model *m = NULL;
    memset(raw, 0, sizeof(raw));
    expect_err(rs_model_decode(317, raw, sizeof(raw), &m), RS_OK, "empty model");
    expect(m && rs_model_vertex_count(m) == 0 && rs_model_face_count(m) == 0, "empty counts");
    expect(m && m->priority == 0 && m->textured_count == 0, "empty priority");
    expect(m && !m->vertex_x && !m->face_colour && !m->face_priority, "empty arrays");
    rs_model_free(m);
    expect_err(rs_model_decode(254, raw, sizeof(raw), &m), RS_OK, "model 254");
    rs_model_free(m);
    expect_err(rs_model_decode(377, raw, sizeof(raw), &m), RS_OK, "model 377");
    rs_model_free(m);
    expect_err(rs_model_decode(474, raw, sizeof(raw), &m), RS_OK, "model 474 old");
    rs_model_free(m);
    expect_err(rs_model_decode(530, raw, sizeof(raw), &m), RS_OK, "model 530 old");
    rs_model_free(m);
    expect_err(rs_model_decode(317, raw, 17, &m), RS_ERR_FORMAT, "short model");
    raw[17] = 1;
    expect_err(rs_model_decode(317, raw, 18, &m), RS_ERR_FORMAT, "length mismatch");
    expect_err(rs_model_decode(530, raw, 18, &m), RS_ERR_FORMAT, "model 530 length");
    expect_err(rs_model_decode(910, raw, 18, &m), RS_ERR_UNSUPPORTED, "model 910");
    expect_err(rs_model_decode(742, raw, 18, &m), RS_ERR_UNSUPPORTED, "model 742");
    {
        uint8_t neu[23];
        memset(neu, 0, sizeof(neu));
        neu[21] = 0xff;
        neu[22] = 0xff;
        expect_err(rs_model_decode(474, neu, sizeof(neu), &m), RS_ERR_UNSUPPORTED, "model 474 new");
        expect_err(rs_model_decode(530, neu, sizeof(neu), &m), RS_ERR_UNSUPPORTED, "model 530 new");
    }
    expect_err(rs_model_decode(317, NULL, 18, &m), RS_ERR_ARG, "model null");
    expect(rs_model_vertex_count(NULL) == 0 && rs_model_face_count(NULL) == 0, "null counts");
    rs_model_free(NULL);
}

/*
 * One vertex, x delta encoded as a 2-byte gsmart of 100 (0xC064).
 * Header: vc=1, fc=0, xlen=2, everything else 0.
 */
static void test_model_smart(void) {
    static const uint8_t raw[] = {
        0x01, 0xc0, 0x64,
        0x00, 0x01,
        0x00, 0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00, 0x02,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00
    };
    rs_model *m = NULL;
    expect_err(rs_model_decode(317, raw, sizeof(raw), &m), RS_OK, "smart model");
    expect(m && m->vertex_count == 1 && m->vertex_x && m->vertex_x[0] == 100, "smart x");
    expect(m && m->vertex_y[0] == 0 && m->vertex_z[0] == 0 && m->face_count == 0, "smart yz");
    rs_model_free(m);
}

/*
 * Four vertices at the origin and four faces, orientation types 1..4.
 * Chain from Model's face-vertex reader:
 *   type 1 smarts 0,1,1 -> 0,1,2
 *   type 2 smart 1      -> 0,2,3
 *   type 3 smart 0      -> 3,2,3
 *   type 4 smart 0      -> 2,3,3  (B is the pre-swap A)
 * Shared priority 0. Colours 1,2,3,4.
 */
static void test_model_faces(void) {
    static const uint8_t raw[] = {
        0x00, 0x00, 0x00, 0x00,
        0x01, 0x02, 0x03, 0x04,
        0x40, 0x41, 0x41, 0x41, 0x40, 0x40,
        0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04,
        0x00, 0x04,
        0x00, 0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x06
    };
    rs_model *m = NULL;
    expect_err(rs_model_decode(317, raw, sizeof(raw), &m), RS_OK, "face model");
    expect(m && m->vertex_count == 4 && m->face_count == 4, "face counts");
    expect(m && m->vertex_x[3] == 0 && m->vertex_y[3] == 0 && m->vertex_z[3] == 0, "origin");
    expect(m && m->face_a[0] == 0 && m->face_b[0] == 1 && m->face_c[0] == 2, "ori 1");
    expect(m && m->face_a[1] == 0 && m->face_b[1] == 2 && m->face_c[1] == 3, "ori 2");
    expect(m && m->face_a[2] == 3 && m->face_b[2] == 2 && m->face_c[2] == 3, "ori 3");
    expect(m && m->face_a[3] == 2 && m->face_b[3] == 3 && m->face_c[3] == 3, "ori 4");
    expect(m && m->face_colour[0] == 1 && m->face_colour[3] == 4, "colours");
    expect(m && !m->face_priority && m->priority == 0 && !m->face_info, "shared priority");
    rs_model_free(m);
}

/*
 * Triangle (0,0,0)-(5,0,0)-(5,0,7), per-face priority 7, face info 2
 * (textured, axis 0), colour/texture id 50, axis vertices 0,1,2.
 */
static void test_model_textured(void) {
    static const uint8_t raw[] = {
        0x00, 0x01, 0x04,
        0x01,
        0x07,
        0x02,
        0x40, 0x41, 0x41,
        0x00, 0x32,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x02,
        0x45,
        0x47,
        0x00, 0x03,
        0x00, 0x01,
        0x01,
        0x01,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00, 0x01,
        0x00, 0x00,
        0x00, 0x01,
        0x00, 0x03
    };
    rs_model *m = NULL;
    expect_err(rs_model_decode(317, raw, sizeof(raw), &m), RS_OK, "textured model");
    expect(m && m->vertex_count == 3 && m->face_count == 1 && m->textured_count == 1, "tex counts");
    expect(m && m->vertex_x[0] == 0 && m->vertex_x[1] == 5 && m->vertex_x[2] == 5, "tex x");
    expect(m && m->vertex_y[0] == 0 && m->vertex_y[1] == 0 && m->vertex_y[2] == 0, "tex y");
    expect(m && m->vertex_z[0] == 0 && m->vertex_z[1] == 0 && m->vertex_z[2] == 7, "tex z");
    expect(m && m->face_a[0] == 0 && m->face_b[0] == 1 && m->face_c[0] == 2, "tex face");
    expect(m && m->face_colour[0] == 50 && m->face_info && m->face_info[0] == 2, "tex id");
    expect(m && m->face_priority && m->face_priority[0] == 7 && m->priority == 0, "tex pri");
    expect(m && m->textured_a[0] == 0 && m->textured_b[0] == 1 && m->textured_c[0] == 2, "tex axis");
    rs_model_free(m);
}

static void put_u8(uint8_t *d, size_t *n, int v) {
    d[(*n)++] = (uint8_t)v;
}

static void put_u16(uint8_t *d, size_t *n, int v) {
    d[(*n)++] = (uint8_t)(v >> 8);
    d[(*n)++] = (uint8_t)v;
}

static void put_str(uint8_t *d, size_t *n, const char *s) {
    size_t i;
    for (i = 0; s[i]; i++) {
        d[(*n)++] = (uint8_t)s[i];
    }
    d[(*n)++] = 10;
}

/* Type 4 text component: width 40, text "Hi", colour 0x00ffffff. */
static size_t build_type4(uint8_t *d) {
    size_t n = 0;
    put_u8(d, &n, 4);
    put_u8(d, &n, 0);
    put_u16(d, &n, 0);
    put_u16(d, &n, 40);
    put_u16(d, &n, 12);
    put_u8(d, &n, 0);
    put_u8(d, &n, 0);
    put_u8(d, &n, 0);
    put_u8(d, &n, 0);
    put_u8(d, &n, 1);
    put_u8(d, &n, 1);
    put_u8(d, &n, 1);
    put_str(d, &n, "Hi");
    put_str(d, &n, "Bye");
    put_u16(d, &n, 0);
    put_u16(d, &n, 0xffff);
    put_u16(d, &n, 0);
    put_u16(d, &n, 2);
    put_u16(d, &n, 0);
    put_u16(d, &n, 3);
    put_u16(d, &n, 0);
    put_u16(d, &n, 4);
    return n;
}

static void test_component_type4(void) {
    uint8_t raw[64];
    size_t n = build_type4(raw);
    rs_buf b;
    rs_component c;
    rs_buf_wrap(&b, raw, n);
    expect_err(rs_component_decode(317, &b, &c), RS_OK, "type4");
    expect(c.type == 4 && c.width == 40 && c.height == 12, "type4 size");
    expect(c.text && strcmp(c.text, "Hi") == 0, "type4 text");
    expect(c.active_text && strcmp(c.active_text, "Bye") == 0, "type4 active");
    expect(c.center == 1 && c.font == 1 && c.shadowed == 1, "type4 font");
    expect(c.colour == 0xffff && c.active_colour == 2 && c.overlayer == -1, "type4 colour");
    expect(c.layer == -1 && c.child_count == 0 && b.pos == n, "type4 tail");
    rs_component_free(&c);
    rs_buf_wrap(&b, raw, n);
    expect_err(rs_component_decode(254, &b, &c), RS_OK, "type4 254");
    expect(c.type == 4 && c.text && strcmp(c.text, "Hi") == 0, "type4 254 text");
    rs_component_free(&c);
    expect_err(rs_component_decode(910, &b, &c), RS_ERR_UNSUPPORTED, "component 910");
    expect_err(rs_component_decode(317, &b, NULL), RS_ERR_ARG, "component null");
}

/* Type 3 button with an empty option string becomes "Ok". */
static void test_component_option(void) {
    uint8_t raw[32];
    size_t n = 0;
    rs_buf b;
    rs_component c;
    put_u8(raw, &n, 3);
    put_u8(raw, &n, 1);
    put_u16(raw, &n, 9);
    put_u16(raw, &n, 8);
    put_u16(raw, &n, 8);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 1);
    put_u16(raw, &n, 0);
    put_u16(raw, &n, 1);
    put_u16(raw, &n, 0);
    put_u16(raw, &n, 2);
    put_u16(raw, &n, 0);
    put_u16(raw, &n, 3);
    put_u16(raw, &n, 0);
    put_u16(raw, &n, 4);
    put_u8(raw, &n, 10);
    rs_buf_wrap(&b, raw, n);
    expect_err(rs_component_decode(317, &b, &c), RS_OK, "option");
    expect(c.type == 3 && c.width == 8 && c.button_type == 1 && c.fill == 1, "option fields");
    expect(c.client_code == 9 && c.option && strcmp(c.option, "Ok") == 0, "option Ok");
    expect(c.colour == 1 && c.active_over_colour == 4 && b.pos == n, "option colour");
    rs_component_free(&c);
}

/*
 * Archive: index length 12, parent 7, id 3, a type-0 layer with one
 * script, one comparator and two children. A second record keeps layer 7.
 */
static void test_interface(void) {
    uint8_t raw[128];
    size_t n = 0;
    rs_component stack[4];
    rs_component *heap = NULL;
    int count = -1;
    rs_component c;

    put_u16(raw, &n, 12);
    put_u16(raw, &n, 65535);
    put_u16(raw, &n, 7);
    put_u16(raw, &n, 3);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 0);
    put_u16(raw, &n, 0);
    put_u16(raw, &n, 100);
    put_u16(raw, &n, 50);
    put_u8(raw, &n, 5);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 1);
    put_u8(raw, &n, 2);
    put_u16(raw, &n, 9);
    put_u8(raw, &n, 1);
    put_u16(raw, &n, 3);
    put_u16(raw, &n, 1);
    put_u16(raw, &n, 2);
    put_u16(raw, &n, 3);
    put_u16(raw, &n, 20);
    put_u8(raw, &n, 1);
    put_u16(raw, &n, 2);
    put_u16(raw, &n, 10);
    put_u16(raw, &n, 1);
    put_u16(raw, &n, 65535);
    put_u16(raw, &n, 11);
    put_u16(raw, &n, 2);
    put_u16(raw, &n, 65534);
    put_u16(raw, &n, 4);
    n += build_type4(raw + n);

    memset(stack, 0, sizeof(stack));
    expect_err(rs_component_archive_decode(317, raw, n, stack, 4, &count), RS_OK, "archive");
    expect(count == 2, "archive count");
    c = stack[0];
    expect(c.id == 3 && c.layer == 7 && c.type == 0 && c.width == 100 && c.height == 50, "layer fields");
    expect(c.alpha == 5 && c.hide == 1 && c.scroll == 20 && c.overlayer == -1, "layer flags");
    expect(c.script_n == 1 && c.script_comparator && c.script_comparator[0] == 2, "cmp");
    expect(c.script_operand && c.script_operand[0] == 9, "operand");
    expect(c.script_count == 1 && c.script_len && c.script_len[0] == 3, "script len");
    expect(c.script_code && c.script_code[0] == 1 && c.script_code[1] == 2 && c.script_code[2] == 3, "script");
    expect(c.child_count == 2 && c.child_id[0] == 10 && c.child_id[1] == 11, "children");
    expect(c.child_x[0] == 1 && c.child_y[0] == -1 && c.child_x[1] == 2 && c.child_y[1] == -2, "child pos");
    expect(stack[1].id == 4 && stack[1].layer == 7 && stack[1].type == 4, "second sticks");
    expect(stack[1].text && strcmp(stack[1].text, "Hi") == 0 && stack[1].width == 40, "second text");
    rs_component_free(&stack[0]);
    rs_component_free(&stack[1]);

    expect_err(rs_interface_decode(317, raw, n, &heap, &count), RS_OK, "interface");
    expect(count == 2 && heap && heap[0].id == 3 && heap[1].width == 40, "interface fields");
    expect(heap[0].child_y[1] == -2 && heap[1].text && strcmp(heap[1].text, "Hi") == 0, "interface body");
    rs_interface_free(heap, count);
    expect_err(rs_interface_decode(254, raw, n, &heap, &count), RS_OK, "interface 254");
    expect(count == 2 && heap && heap[0].id == 3 && heap[1].text && strcmp(heap[1].text, "Hi") == 0,
           "interface 254 body");
    rs_interface_free(heap, count);

    expect_err(rs_component_archive_decode(317, raw, n, stack, 1, &count), RS_ERR_RANGE, "archive cap");
    expect(count == 0 && stack[0].text == NULL && stack[0].child_id == NULL, "archive cleared");

    expect_err(rs_interface_decode(910, raw, n, &heap, &count), RS_ERR_UNSUPPORTED, "interface 910");
    expect_err(rs_interface_decode(742, raw, n, &heap, &count), RS_ERR_UNSUPPORTED, "interface 742");
    {
        uint8_t empty[2] = {0, 0};
        expect_err(rs_interface_decode(317, empty, 2, &heap, &count), RS_OK, "empty interface");
        expect(count == 0 && heap == NULL, "empty count");
        rs_interface_free(heap, count);
    }
    rs_interface_free(NULL, 0);
}

static void test_type2_actions(void) {
    uint8_t raw[64];
    size_t n = 0;
    int i;
    rs_buf b;
    rs_component c;
    put_u8(raw, &n, 2);
    put_u8(raw, &n, 0);
    put_u16(raw, &n, 0);
    put_u16(raw, &n, 1);
    put_u16(raw, &n, 1);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 1);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 1);
    put_u8(raw, &n, 0);
    put_u8(raw, &n, 4);
    put_u8(raw, &n, 6);
    for (i = 0; i < 20; i++) {
        put_u8(raw, &n, 0);
    }
    put_str(raw, &n, "Take");
    put_str(raw, &n, "");
    put_str(raw, &n, "");
    put_str(raw, &n, "");
    put_str(raw, &n, "Drop");
    put_str(raw, &n, "Use");
    put_str(raw, &n, "Item");
    put_u16(raw, &n, 3);
    rs_buf_wrap(&b, raw, n);
    expect_err(rs_component_decode(317, &b, &c), RS_OK, "type2");
    expect(c.type == 2 && c.width == 1 && c.draggable == 1 && c.swappable == 0, "type2 flags");
    expect(c.margin_x == 4 && c.margin_y == 6, "type2 margin");
    expect(c.action[0] && strcmp(c.action[0], "Take") == 0 && c.action[1] == NULL, "type2 actions");
    expect(c.action[4] && strcmp(c.action[4], "Drop") == 0, "type2 drop");
    expect(c.target_verb && strcmp(c.target_verb, "Use") == 0, "type2 verb");
    expect(c.target_text && strcmp(c.target_text, "Item") == 0 && c.target_mask == 3, "type2 target");
    expect(b.pos == n, "type2 consumed");
    rs_component_free(&c);
}

int main(void) {
    test_model_empty();
    test_model_smart();
    test_model_faces();
    test_model_textured();
    test_component_type4();
    test_component_option();
    test_type2_actions();
    test_interface();
    if (g_failed) {
        fprintf(stderr, "test_asset failed\n");
        return 1;
    }
    printf("test_asset ok\n");
    return 0;
}
