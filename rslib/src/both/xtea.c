#include "internal.h"

#define XTEA_DELTA 0x9E3779B9u
#define XTEA_SUM 0xC6EF3720u

void rs_xtea_encrypt(uint32_t key[4], uint8_t *data, size_t len) {
    size_t i;
    for (i = 0; i + 8 <= len; i += 8) {
        uint32_t v0 = rs_be32u(data + i);
        uint32_t v1 = rs_be32u(data + i + 4);
        uint32_t sum = 0;
        int r;
        for (r = 0; r < 32; r++) {
            v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (key[sum & 3] + sum);
            sum += XTEA_DELTA;
            v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (key[(sum >> 11) & 3] + sum);
        }
        rs_put_be32(data + i, v0);
        rs_put_be32(data + i + 4, v1);
    }
}

void rs_xtea_decrypt_range(uint32_t key[4], uint8_t *data, size_t off, size_t end) {
    size_t i;
    for (i = off; i + 8 <= end; i += 8) {
        uint32_t v0 = rs_be32u(data + i);
        uint32_t v1 = rs_be32u(data + i + 4);
        uint32_t sum = XTEA_SUM;
        int r;
        for (r = 0; r < 32; r++) {
            v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (key[(sum >> 11) & 3] + sum);
            sum -= XTEA_DELTA;
            v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (key[sum & 3] + sum);
        }
        rs_put_be32(data + i, v0);
        rs_put_be32(data + i + 4, v1);
    }
}

void rs_xtea_decrypt(uint32_t key[4], uint8_t *data, size_t len) {
    rs_xtea_decrypt_range(key, data, 0, len);
}
