#include "internal.h"

static uint32_t g_crc[256];
static int g_crc_ready;

static void crc_init(void) {
    int i, j;
    if (g_crc_ready) {
        return;
    }
    for (i = 0; i < 256; i++) {
        uint32_t c = (uint32_t)i;
        for (j = 0; j < 8; j++) {
            if (c & 1) {
                c = (c >> 1) ^ 0xEDB88320u;
            } else {
                c >>= 1;
            }
        }
        g_crc[i] = c;
    }
    g_crc_ready = 1;
}

uint32_t rs_crc32_update(uint32_t crc, const void *data, size_t len) {
    const uint8_t *p = data;
    size_t i;
    uint32_t c;
    crc_init();
    c = crc ^ 0xFFFFFFFFu;
    for (i = 0; i < len; i++) {
        c = (c >> 8) ^ g_crc[(c ^ p[i]) & 0xff];
    }
    return c ^ 0xFFFFFFFFu;
}

uint32_t rs_crc32(const void *data, size_t len) {
    return rs_crc32_update(0, data, len);
}
