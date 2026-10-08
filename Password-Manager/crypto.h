/* crypto.h - SHA-256, HMAC, PBKDF2, stream cipher (educational implementation) */
#ifndef CRYPTO_H
#define CRYPTO_H
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t state[8];
    uint64_t bitlen;
    uint8_t  buf[64];
    size_t   buflen;
} Sha256;

void sha256_init(Sha256 *c);
void sha256_update(Sha256 *c, const uint8_t *data, size_t len);
void sha256_final(Sha256 *c, uint8_t out[32]);
void sha256(const uint8_t *data, size_t len, uint8_t out[32]);

void hmac_sha256(const uint8_t *key, size_t klen,
                 const uint8_t *msg, size_t mlen, uint8_t out[32]);

/* PBKDF2-HMAC-SHA256 (single 32-byte block) : password -> key */
void derive_key(const char *password, const uint8_t salt[16],
                uint32_t iterations, uint8_t out[32]);

/* HMAC-SHA256 in counter mode used as a keystream. Same call encrypts/decrypts. */
void stream_crypt(const uint8_t key[32], const uint8_t nonce[16],
                  uint8_t *data, size_t len);

int  random_bytes(uint8_t *buf, size_t n);          /* OS secure RNG, 0 = ok */
int  ct_equal(const uint8_t *a, const uint8_t *b, size_t n); /* constant time */
void secure_zero(void *p, size_t n);
#endif