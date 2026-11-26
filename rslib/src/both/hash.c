#include "internal.h"

int32_t rs_hash_jag(const char *name) {
    int32_t h = 0;
    for (; name && *name; name++) {
        unsigned char c = (unsigned char)*name;
        if (c >= 'a' && c <= 'z') {
            c = (unsigned char)(c - 32);
        }
        h = h * 61 + (int32_t)c - 32;
    }
    return h;
}

int32_t rs_hash_js5(const char *name) {
    int32_t h = 0;
    for (; name && *name; name++) {
        h = (int32_t)(unsigned char)*name + ((h << 5) - h);
    }
    return h;
}
