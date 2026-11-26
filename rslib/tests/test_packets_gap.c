#include "rslib/both/buf.h"
#include "rslib/both/zonebuf.h"

#include <stdio.h>
#include <string.h>

extern const char *rs_packet_wire_report(void);

static int fail(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    return 1;
}

/* Client read order: header bytes, then the raw opcode, then the body. */
static int expect_tail(const rs_buf *b, size_t header, int opcode, const uint8_t *body, size_t n) {
    size_t i;
    if (b->pos < header + 1 + n) {
        return fail("short payload");
    }
    if (b->data[header] != (uint8_t)opcode) {
        return fail("opcode not where the client reads it");
    }
    for (i = 0; i < n; i++) {
        if (b->data[header + 1 + i] != body[i]) {
            return fail("body not where the client reads it");
        }
    }
    return 0;
}

static int check_rev(int revision, int base_x, int base_z, size_t header, const uint8_t *expect_hdr) {
    rs_zonebuf *z;
    rs_buf dst;
    uint8_t body[3] = {0x11, 0x22, 0x33};
    int opcode = 49;
    size_t i;
    int rc;

    z = rs_zonebuf_create(revision);
    if (!z) {
        return fail("create");
    }
    if (rs_zonebuf_begin(z, base_x, base_z) != RS_OK) {
        rs_zonebuf_destroy(z);
        return fail("begin");
    }
    if (rs_zonebuf_append(z, opcode, body, 3) != RS_OK) {
        rs_zonebuf_destroy(z);
        return fail("append");
    }
    if (rs_buf_alloc(&dst, 64) != RS_OK) {
        rs_zonebuf_destroy(z);
        return fail("buf");
    }
    if (rs_zonebuf_finish(z, &dst) != RS_OK) {
        rs_buf_free(&dst);
        rs_zonebuf_destroy(z);
        return fail("finish");
    }
    for (i = 0; i < header; i++) {
        if (dst.data[i] != expect_hdr[i]) {
            rs_buf_free(&dst);
            rs_zonebuf_destroy(z);
            return fail("header bytes");
        }
    }
    rc = expect_tail(&dst, header, opcode, body, 3);
    rs_buf_free(&dst);
    rs_zonebuf_destroy(z);
    return rc;
}

int main(void) {
    rs_zonebuf *z317;
    rs_buf dst;
    const char *report;
    /* 474: g1(10), g1_sub128(20) = 108, then opcode. */
    uint8_t h474[2] = {10, (uint8_t)(128 - 20)};
    /* 530: g1_add128(z=20) = 148, g1_sub128(x=10) = 118. */
    uint8_t h530[2] = {(uint8_t)(20 + 128), (uint8_t)(128 - 10)};
    /* 742: level g1_alt3(0) = 128, z g1b_alt2(24>>3=3) = -3, x g1b(16>>3=2) = 2. */
    uint8_t h742[3] = {128, (uint8_t)(-3), 2};
    /* 910: level g1_alt1(0) = 128, z and x both g1b_alt2. */
    uint8_t h910[3] = {128, (uint8_t)(-3), (uint8_t)(-2)};

    /* rs_sizes_c2s_282 was not added: the client has no c2s size array. */

    z317 = rs_zonebuf_create(317);
    if (!z317) {
        return fail("create 317");
    }
    if (rs_buf_alloc(&dst, 32) != RS_OK) {
        rs_zonebuf_destroy(z317);
        return fail("buf 317");
    }
    if (rs_zonebuf_finish(z317, &dst) != RS_ERR_UNSUPPORTED) {
        rs_buf_free(&dst);
        rs_zonebuf_destroy(z317);
        return fail("317 finish");
    }
    rs_buf_free(&dst);
    rs_zonebuf_destroy(z317);

    if (check_rev(474, 10, 20, 2, h474) || check_rev(530, 10, 20, 2, h530) ||
        check_rev(742, 16, 24, 3, h742) || check_rev(910, 16, 24, 3, h910)) {
        return 1;
    }

    report = rs_packet_wire_report();
    if (!report || !strstr(report, "578") || !strstr(report, "831") || !strstr(report, "860") ||
        !strstr(report, "none")) {
        return fail("wire report");
    }
    return 0;
}
