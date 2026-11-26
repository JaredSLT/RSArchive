#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"

uint64_t rs_rand_seed(void) {
    uint64_t t = rs_now_ms();
    t ^= t << 13;
    t ^= t >> 7;
    t ^= t << 17;
    return t;
}
