#include "rslib.h"
#include "rslib/both/login_ext.h"

#include <stdio.h>
#include <string.h>

static int g_hits;
static int g_archive;
static int g_group;
static uint8_t g_box[1024];
static size_t g_n;

static void on_group(void *user, int archive, int group, const uint8_t *container, size_t n) {
    (void)user;
    g_hits++;
    g_archive = archive;
    g_group = group;
    g_n = n < sizeof(g_box) ? n : sizeof(g_box);
    memcpy(g_box, container, g_n);
}

static int fail(const char *msg) {
    fprintf(stderr, "test_session_gap: %s\n", msg);
    return 1;
}

int main(void) {
    uint8_t container[9] = {0, 0, 0, 0, 4, 'a', 'b', 'c', 'd'};
    rs_buf dst;
    rs_js5_reply st;
    const rs_build_info *b = rs_build_lookup(474);
    int crc = -1;
    size_t mid;
    int i;

    if (!b) {
        return fail("no 474 build");
    }
    if (rs_buf_alloc(&dst, 64) != RS_OK) {
        return fail("alloc");
    }
    if (rs_js5_write_group(b, &dst, 2, 9, container, sizeof(container), 0) != RS_OK) {
        return fail("write group");
    }
    if (dst.pos != 12) {
        return fail("474 frame is not 8+4");
    }
    memset(&st, 0, sizeof(st));
    mid = 10; /* header plus the first half of the 4-byte payload */
    if (rs_js5_reply_feed(&st, 474, dst.data, mid, on_group, NULL) != RS_OK || g_hits != 0) {
        return fail("partial feed delivered a group");
    }
    if (rs_js5_reply_feed(&st, 474, dst.data + mid, dst.pos - mid, on_group, NULL) != RS_OK) {
        return fail("second feed");
    }
    if (g_hits != 1 || g_archive != 2 || g_group != 9 || g_n != sizeof(container) ||
        memcmp(g_box, container, sizeof(container)) != 0) {
        return fail("reassembled container");
    }
    rs_js5_reply_free(&st);

    /* Same frame, XOR'd, one shot. */
    for (i = 0; i < (int)dst.pos; i++) {
        dst.data[i] = (uint8_t)(dst.data[i] ^ 0x5a);
    }
    memset(&st, 0, sizeof(st));
    st.encrypt = 1;
    st.key = 0x5a;
    g_hits = 0;
    if (rs_js5_reply_feed(&st, 474, dst.data, dst.pos, on_group, NULL) != RS_OK || g_hits != 1 ||
        memcmp(g_box, container, sizeof(container)) != 0) {
        return fail("xor undo");
    }
    rs_js5_reply_free(&st);
    rs_buf_free(&dst);

    {
        uint8_t big[5 + 520];
        size_t k;
        memset(big, 0x11, sizeof(big));
        big[0] = 0;
        big[1] = 0;
        big[2] = 0;
        big[3] = 2;
        big[4] = 8; /* clen 520 */
        if (rs_buf_alloc(&dst, 1024) != RS_OK) {
            return fail("big alloc");
        }
        if (rs_js5_write_group(b, &dst, 4, 7, big, sizeof(big), 0) != RS_OK) {
            return fail("big write");
        }
        /* 8 header + 520 payload, one 0xFF after 512 data bytes. */
        if (dst.pos != 8 + 520 + 1) {
            return fail("0xFF not inserted");
        }
        memset(&st, 0, sizeof(st));
        g_hits = 0;
        k = dst.pos / 2;
        if (rs_js5_reply_feed(&st, 474, dst.data, k, on_group, NULL) != RS_OK || g_hits != 0) {
            return fail("big partial");
        }
        if (rs_js5_reply_feed(&st, 474, dst.data + k, dst.pos - k, on_group, NULL) != RS_OK ||
            g_hits != 1 || g_archive != 4 || g_group != 7 || g_n != sizeof(big) ||
            memcmp(g_box, big, sizeof(big)) != 0) {
            return fail("big reassembly");
        }
        rs_js5_reply_free(&st);
        rs_buf_free(&dst);
    }

    if (rs_login_facts(317, &crc, NULL, NULL) != RS_OK || crc != 9) {
        return fail("facts 317");
    }
    if (rs_login_facts(831, &crc, NULL, NULL) != RS_ERR_NOT_FOUND) {
        return fail("facts 831");
    }
    return 0;
}
