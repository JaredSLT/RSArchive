#include "internal.h"

static const char k_b37[] = "_abcdefghijklmnopqrstuvwxyz0123456789";

uint64_t rs_base37_encode(const char *name) {
    uint64_t v = 0;
    int i;
    for (i = 0; name && name[i] && i < 12; i++) {
        char c = name[i];
        v *= 37;
        if (c >= 'A' && c <= 'Z') {
            v += (uint64_t)(1 + c - 'A');
        } else if (c >= 'a' && c <= 'z') {
            v += (uint64_t)(1 + c - 'a');
        } else if (c >= '0' && c <= '9') {
            v += (uint64_t)(27 + c - '0');
        }
    }
    while (v % 37 == 0 && v != 0) {
        v /= 37;
    }
    return v;
}

void rs_base37_decode(uint64_t v, char out[13]) {
    char tmp[12];
    int n = 0;
    int i;
    if (v == 0 || v >= 6582952005840035281ULL || v % 37 == 0) {
        memcpy(out, "invalid_name", 13);
        return;
    }
    while (v != 0 && n < 12) {
        uint64_t prev = v;
        v /= 37;
        tmp[n++] = k_b37[(int)(prev - v * 37)];
    }
    for (i = 0; i < n; i++) {
        out[i] = tmp[n - 1 - i];
    }
    out[n] = 0;
}
