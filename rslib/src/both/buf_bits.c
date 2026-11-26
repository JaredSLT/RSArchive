#include "internal.h"

static const int k_bitmask[33] = {
    0,          1,          3,          7,          15,         31,
    63,         127,        255,        511,        1023,       2047,
    4095,       8191,       16383,      32767,      65535,      131071,
    262143,     524287,     1048575,    2097151,    4194303,    8388607,
    16777215,   33554431,   67108863,   134217727,  268435455,  536870911,
    1073741823, 2147483647, -1};

void rs_access_bits(rs_buf *b) {
    b->bit_pos = b->pos * 8;
}

void rs_access_bytes(rs_buf *b) {
    b->pos = (b->bit_pos + 7) / 8;
}

int rs_gbit(rs_buf *b, int n) {
    int off = (int)(b->bit_pos >> 3);
    int rem = 8 - (int)(b->bit_pos & 7);
    int v = 0;
    b->bit_pos += (size_t)n;
    while (n > rem) {
        v += (b->data[off++] & k_bitmask[rem]) << (n - rem);
        n -= rem;
        rem = 8;
    }
    if (n == rem) {
        v += b->data[off] & k_bitmask[rem];
    } else {
        v += (b->data[off] >> (rem - n)) & k_bitmask[n];
    }
    return v;
}

rs_err rs_pbit(rs_buf *b, int n, int v) {
    int off, rem;
    rs_err e = rs_buf_ensure(b, (n + 7) / 8 + 1);
    if (e != RS_OK) {
        return e;
    }
    off = (int)(b->bit_pos >> 3);
    rem = 8 - (int)(b->bit_pos & 7);
    b->bit_pos += (size_t)n;
    while (n > rem) {
        b->data[off] &= (uint8_t)~k_bitmask[rem];
        b->data[off++] |= (uint8_t)((v >> (n - rem)) & k_bitmask[rem]);
        n -= rem;
        rem = 8;
    }
    if (n == rem) {
        b->data[off] &= (uint8_t)~k_bitmask[rem];
        b->data[off] |= (uint8_t)(v & k_bitmask[rem]);
    } else {
        b->data[off] &= (uint8_t)~(k_bitmask[n] << (rem - n));
        b->data[off] |= (uint8_t)((v & k_bitmask[n]) << (rem - n));
    }
    if (b->bit_pos / 8 > b->pos) {
        b->pos = (b->bit_pos + 7) / 8;
    }
    return RS_OK;
}
