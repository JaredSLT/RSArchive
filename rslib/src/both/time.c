#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdint.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <time.h>
#endif

#ifdef _WIN32
uint64_t rs_now_ms(void) {
    static LARGE_INTEGER freq;
    static uint64_t last;
    LARGE_INTEGER now;
    uint64_t q;
    uint64_t f;
    uint64_t ms;

    /* Windows 95 has no clock_gettime. */
    if (freq.QuadPart == 0) {
        if (!QueryPerformanceFrequency(&freq) || freq.QuadPart <= 0) {
            freq.QuadPart = 0;
            return last;
        }
    }
    if (!QueryPerformanceCounter(&now) || now.QuadPart < 0) {
        return last;
    }
    q = (uint64_t)now.QuadPart;
    f = (uint64_t)freq.QuadPart;
    ms = (q / f) * 1000ull + ((q % f) * 1000ull) / f;
    if (ms < last) {
        ms = last;
    }
    last = ms;
    return ms;
}
#else
uint64_t rs_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ull + (uint64_t)ts.tv_nsec / 1000000ull;
}
#endif
