#include "rslib/both/crypto.h"

#include <string.h>

/* FIPS 180-4 SHA-256. HMAC is RFC 2104. PBKDF2 is PKCS #5 / RFC 8018. */

enum { SHA256_BLOCK = 64 };

typedef struct sha256_ctx {
    uint32_t state[8];
    uint64_t nbytes;
    uint8_t buf[SHA256_BLOCK];
    size_t buflen;
} sha256_ctx;

static uint32_t rotr(uint32_t x, unsigned n) {
    return (x >> n) | (x << (32u - n));
}

static const uint32_t K[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u,
    0xab1c5ed5u, 0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu,
    0x9bdc06a7u, 0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu,
    0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u,
    0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u, 0xa2bfe8a1u, 0xa81a664bu,
    0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u,
    0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
    0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u,
    0xc67178f2u
};

static void sha256_block(sha256_ctx *ctx, const uint8_t blk[SHA256_BLOCK]) {
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    int t;

    for (t = 0; t < 16; t++) {
        w[t] = ((uint32_t)blk[t * 4] << 24) | ((uint32_t)blk[t * 4 + 1] << 16) |
               ((uint32_t)blk[t * 4 + 2] << 8) | (uint32_t)blk[t * 4 + 3];
    }
    for (t = 16; t < 64; t++) {
        uint32_t s0 = rotr(w[t - 15], 7) ^ rotr(w[t - 15], 18) ^ (w[t - 15] >> 3);
        uint32_t s1 = rotr(w[t - 2], 17) ^ rotr(w[t - 2], 19) ^ (w[t - 2] >> 10);
        w[t] = w[t - 16] + s0 + w[t - 7] + s1;
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];
    for (t = 0; t < 64; t++) {
        uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t t1 = h + S1 + ch + K[t] + w[t];
        uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = S0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

static void sha256_init(sha256_ctx *ctx) {
    ctx->state[0] = 0x6a09e667u;
    ctx->state[1] = 0xbb67ae85u;
    ctx->state[2] = 0x3c6ef372u;
    ctx->state[3] = 0xa54ff53au;
    ctx->state[4] = 0x510e527fu;
    ctx->state[5] = 0x9b05688cu;
    ctx->state[6] = 0x1f83d9abu;
    ctx->state[7] = 0x5be0cd19u;
    ctx->nbytes = 0;
    ctx->buflen = 0;
}

/* Does not touch nbytes. Padding calls this after the message length is fixed. */
static void sha256_absorb(sha256_ctx *ctx, const uint8_t *data, size_t len) {
    size_t i = 0;
    if (!len) {
        return;
    }
    if (ctx->buflen) {
        size_t n = SHA256_BLOCK - ctx->buflen;
        if (n > len) {
            n = len;
        }
        memcpy(ctx->buf + ctx->buflen, data, n);
        ctx->buflen += n;
        i += n;
        if (ctx->buflen == SHA256_BLOCK) {
            sha256_block(ctx, ctx->buf);
            ctx->buflen = 0;
        }
    }
    for (; i + SHA256_BLOCK <= len; i += SHA256_BLOCK) {
        sha256_block(ctx, data + i);
    }
    if (i < len) {
        memcpy(ctx->buf, data + i, len - i);
        ctx->buflen = len - i;
    }
}

static void sha256_final(sha256_ctx *ctx, uint8_t out[32]) {
    uint64_t bits = ctx->nbytes << 3;
    uint8_t lenb[8];
    uint8_t byte;
    int i;

    byte = 0x80;
    sha256_absorb(ctx, &byte, 1);
    byte = 0;
    while (ctx->buflen != 56) {
        sha256_absorb(ctx, &byte, 1);
    }
    for (i = 0; i < 8; i++) {
        lenb[7 - i] = (uint8_t)(bits >> (8 * i));
    }
    sha256_absorb(ctx, lenb, 8);
    for (i = 0; i < 8; i++) {
        out[i * 4] = (uint8_t)(ctx->state[i] >> 24);
        out[i * 4 + 1] = (uint8_t)(ctx->state[i] >> 16);
        out[i * 4 + 2] = (uint8_t)(ctx->state[i] >> 8);
        out[i * 4 + 3] = (uint8_t)ctx->state[i];
    }
}

void rs_sha256(const void *data, size_t len, uint8_t out[32]) {
    sha256_ctx ctx;
    sha256_init(&ctx);
    if (len && data) {
        ctx.nbytes = (uint64_t)len;
        sha256_absorb(&ctx, data, len);
    }
    sha256_final(&ctx, out);
}

static void hmac_sha256(const void *key, size_t key_len, const void *d1, size_t n1, const void *d2,
                        size_t n2, uint8_t out[32]) {
    uint8_t k[SHA256_BLOCK];
    uint8_t tk[32];
    uint8_t pad[SHA256_BLOCK];
    uint8_t inner[32];
    sha256_ctx ctx;
    size_t i;

    memset(k, 0, sizeof(k));
    if (key_len > SHA256_BLOCK) {
        rs_sha256(key, key_len, tk);
        memcpy(k, tk, 32);
    } else if (key_len && key) {
        memcpy(k, key, key_len);
    }

    for (i = 0; i < SHA256_BLOCK; i++) {
        pad[i] = (uint8_t)(k[i] ^ 0x36u);
    }
    sha256_init(&ctx);
    ctx.nbytes = SHA256_BLOCK + (uint64_t)n1 + (uint64_t)n2;
    sha256_absorb(&ctx, pad, SHA256_BLOCK);
    if (n1 && d1) {
        sha256_absorb(&ctx, d1, n1);
    }
    if (n2 && d2) {
        sha256_absorb(&ctx, d2, n2);
    }
    sha256_final(&ctx, inner);

    for (i = 0; i < SHA256_BLOCK; i++) {
        pad[i] = (uint8_t)(k[i] ^ 0x5cu);
    }
    sha256_init(&ctx);
    ctx.nbytes = SHA256_BLOCK + 32;
    sha256_absorb(&ctx, pad, SHA256_BLOCK);
    sha256_absorb(&ctx, inner, 32);
    sha256_final(&ctx, out);
}

void rs_hmac_sha256(const void *key, size_t key_len, const void *data, size_t data_len,
                    uint8_t out[32]) {
    hmac_sha256(key, key_len, data, data_len, NULL, 0, out);
}

void rs_pbkdf2_hmac_sha256(const void *password, size_t password_len, const void *salt,
                           size_t salt_len, uint32_t iterations, uint8_t *out, size_t out_len) {
    uint32_t block;
    size_t done;

    if (!out || out_len == 0) {
        return;
    }
    if (!password) {
        password_len = 0;
    }
    if (!salt) {
        salt_len = 0;
    }
    if (iterations == 0) {
        memset(out, 0, out_len);
        return;
    }

    done = 0;
    block = 1;
    while (done < out_len) {
        uint8_t be[4];
        uint8_t u[32];
        uint8_t t[32];
        uint32_t iter;
        size_t n;
        size_t i;

        be[0] = (uint8_t)(block >> 24);
        be[1] = (uint8_t)(block >> 16);
        be[2] = (uint8_t)(block >> 8);
        be[3] = (uint8_t)block;
        hmac_sha256(password, password_len, salt, salt_len, be, 4, u);
        memcpy(t, u, 32);
        for (iter = 1; iter < iterations; iter++) {
            hmac_sha256(password, password_len, u, 32, NULL, 0, u);
            for (i = 0; i < 32; i++) {
                t[i] ^= u[i];
            }
        }
        n = out_len - done;
        if (n > 32) {
            n = 32;
        }
        memcpy(out + done, t, n);
        done += n;
        if (block == 0xffffffffu) {
            break;
        }
        block++;
    }
}
