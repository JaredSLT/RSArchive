#ifndef RSLIB_CRYPTO_H
#define RSLIB_CRYPTO_H

#include "rslib/both/buf.h"
#include "rslib/both/build.h"
#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t rs_crc32(const void *data, size_t len);
uint32_t rs_crc32_update(uint32_t crc, const void *data, size_t len); /* init 0 */

int32_t rs_hash_jag(const char *name); /* jagfile / RS2 archive names */
int32_t rs_hash_js5(const char *name); /* JS5 group/file names */

uint64_t rs_base37_encode(const char *name);
void rs_base37_decode(uint64_t v, char out[13]);

typedef struct rs_isaac {
    uint32_t mem[256];
    uint32_t rsl[256];
    uint32_t a, b, c;
    int count;
} rs_isaac;

void rs_isaac_init(rs_isaac *is, const uint32_t seed[4]);
uint32_t rs_isaac_next(rs_isaac *is);

void rs_xtea_encrypt(uint32_t key[4], uint8_t *data, size_t len);
void rs_xtea_decrypt(uint32_t key[4], uint8_t *data, size_t len);
/* Skip `off` bytes (JS5 containers skip 5), operate on 8-byte blocks. */
void rs_xtea_decrypt_range(uint32_t key[4], uint8_t *data, size_t off, size_t end);

enum { RS_RSA_MAX_BYTES = 256 };

typedef struct rs_rsa_key {
    uint8_t n[RS_RSA_MAX_BYTES];
    uint8_t e[RS_RSA_MAX_BYTES];
    uint8_t d[RS_RSA_MAX_BYTES];
    int nlen;
    int elen;
    int dlen;
} rs_rsa_key;

rs_err rs_rsa_set_decimal(rs_rsa_key *key, const char *modulus, const char *exponent);
rs_err rs_rsa_set_private_decimal(rs_rsa_key *key, const char *modulus, const char *exponent);
rs_err rs_rsa_modpow(const rs_rsa_key *key, const uint8_t *in, size_t in_len,
                     uint8_t *out, size_t *out_len);
/* Uses d when present, otherwise e. */
rs_err rs_rsa_modpow_key(const rs_rsa_key *key, int use_private, const uint8_t *in,
                         size_t in_len, uint8_t *out, size_t *out_len);

void rs_whirlpool(const void *data, size_t len, uint8_t out[64]);

enum { RS_HUFFMAN_SYMS = 256 };

typedef struct rs_huffman {
    uint8_t bits[RS_HUFFMAN_SYMS];
    uint32_t codes[RS_HUFFMAN_SYMS];
    int tree[RS_HUFFMAN_SYMS * 2];
    int tree_len;
    int ready;
} rs_huffman;

rs_err rs_huffman_init(rs_huffman *h, const uint8_t *bits, size_t nsym);
rs_err rs_huffman_encode(const rs_huffman *h, const char *text, rs_buf *dst);
rs_err rs_huffman_decode(const rs_huffman *h, rs_buf *src, int nbytes, char *dst, size_t dst_cap);

/* 317 WordPack nibble chat (also used as the RS2 fallback). */
rs_err rs_wordpack_encode(const char *text, rs_buf *dst);
rs_err rs_wordpack_decode(rs_buf *src, int nbytes, char *dst, size_t dst_cap);

rs_err rs_chat_encode(const rs_build_info *build, const rs_huffman *h, const char *text,
                      rs_buf *dst);
rs_err rs_chat_decode(const rs_build_info *build, const rs_huffman *h, rs_buf *src, int nbytes,
                      char *dst, size_t dst_cap);

/* FIPS 180-4 SHA-256 and PKCS #5 PBKDF2-HMAC-SHA256. No OpenSSL. */
void rs_sha256(const void *data, size_t len, uint8_t out[32]);
void rs_hmac_sha256(const void *key, size_t key_len, const void *data, size_t data_len,
                    uint8_t out[32]);
void rs_pbkdf2_hmac_sha256(const void *password, size_t password_len, const void *salt,
                           size_t salt_len, uint32_t iterations, uint8_t *out, size_t out_len);

#ifdef __cplusplus
}
#endif

#endif
