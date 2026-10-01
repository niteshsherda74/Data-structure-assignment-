/* crypto.c */
#ifdef _WIN32
#define _CRT_RAND_S
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "crypto.h"

#if defined(_WIN32) && defined(__MINGW32__)
extern int rand_s(unsigned int *);
#endif

static const uint32_t K[64] = {
0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};

#define ROR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

static void transform(Sha256 *c, const uint8_t *d) {
    uint32_t w[64], a, b, cc, dd, e, f, g, h, t1, t2, s0, s1;
    int i;
    for (i = 0; i < 16; i++)
        w[i] = ((uint32_t)d[4*i] << 24) | ((uint32_t)d[4*i+1] << 16) |
               ((uint32_t)d[4*i+2] << 8) | (uint32_t)d[4*i+3];
    for (i = 16; i < 64; i++) {
        s0 = ROR(w[i-15], 7) ^ ROR(w[i-15], 18) ^ (w[i-15] >> 3);
        s1 = ROR(w[i-2], 17) ^ ROR(w[i-2], 19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    a = c->state[0]; b = c->state[1]; cc = c->state[2]; dd = c->state[3];
    e = c->state[4]; f = c->state[5]; g = c->state[6]; h = c->state[7];
    for (i = 0; i < 64; i++) {
        t1 = h + (ROR(e, 6) ^ ROR(e, 11) ^ ROR(e, 25)) + ((e & f) ^ (~e & g)) + K[i] + w[i];
        t2 = (ROR(a, 2) ^ ROR(a, 13) ^ ROR(a, 22)) + ((a & b) ^ (a & cc) ^ (b & cc));
        h = g; g = f; f = e; e = dd + t1; dd = cc; cc = b; b = a; a = t1 + t2;
    }
    c->state[0] += a; c->state[1] += b; c->state[2] += cc; c->state[3] += dd;
    c->state[4] += e; c->state[5] += f; c->state[6] += g; c->state[7] += h;
}

void sha256_init(Sha256 *c) {
    static const uint32_t iv[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
                                   0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    memcpy(c->state, iv, sizeof iv);
    c->bitlen = 0; c->buflen = 0;
}

void sha256_update(Sha256 *c, const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        c->buf[c->buflen++] = data[i];
        if (c->buflen == 64) { transform(c, c->buf); c->bitlen += 512; c->buflen = 0; }
    }
}

void sha256_final(Sha256 *c, uint8_t out[32]) {
    uint64_t total = c->bitlen + (uint64_t)c->buflen * 8;
    c->buf[c->buflen++] = 0x80;
    if (c->buflen > 56) {
        while (c->buflen < 64) c->buf[c->buflen++] = 0;
        transform(c, c->buf); c->buflen = 0;
    }
    while (c->buflen < 56) c->buf[c->buflen++] = 0;
    for (int i = 0; i < 8; i++) c->buf[56 + i] = (uint8_t)(total >> (56 - 8 * i));
    transform(c, c->buf);
    for (int i = 0; i < 8; i++) {
        out[4*i]   = (uint8_t)(c->state[i] >> 24);
        out[4*i+1] = (uint8_t)(c->state[i] >> 16);
        out[4*i+2] = (uint8_t)(c->state[i] >> 8);
        out[4*i+3] = (uint8_t)(c->state[i]);
    }
}

void sha256(const uint8_t *data, size_t len, uint8_t out[32]) {
    Sha256 c; sha256_init(&c); sha256_update(&c, data, len); sha256_final(&c, out);
}

void hmac_sha256(const uint8_t *key, size_t klen,
                 const uint8_t *msg, size_t mlen, uint8_t out[32]) {
    uint8_t k[64] = {0}, ipad[64], opad[64], inner[32];
    Sha256 c;
    if (klen > 64) sha256(key, klen, k); else if (klen) memcpy(k, key, klen);
    for (int i = 0; i < 64; i++) { ipad[i] = k[i] ^ 0x36; opad[i] = k[i] ^ 0x5c; }
    sha256_init(&c); sha256_update(&c, ipad, 64); sha256_update(&c, msg, mlen); sha256_final(&c, inner);
    sha256_init(&c); sha256_update(&c, opad, 64); sha256_update(&c, inner, 32); sha256_final(&c, out);
    secure_zero(k, sizeof k);
}

void derive_key(const char *password, const uint8_t salt[16],
                uint32_t iterations, uint8_t out[32]) {
    size_t pl = strlen(password);
    uint8_t s[20], u[32], t[32], tmp[32];
    memcpy(s, salt, 16); s[16] = 0; s[17] = 0; s[18] = 0; s[19] = 1;
    hmac_sha256((const uint8_t *)password, pl, s, 20, u);
    memcpy(t, u, 32);
    for (uint32_t i = 1; i < iterations; i++) {
        hmac_sha256((const uint8_t *)password, pl, u, 32, tmp);
        memcpy(u, tmp, 32);
        for (int j = 0; j < 32; j++) t[j] ^= u[j];
    }
    memcpy(out, t, 32);
    secure_zero(u, 32); secure_zero(t, 32); secure_zero(tmp, 32);
}

void stream_crypt(const uint8_t key[32], const uint8_t nonce[16],
                  uint8_t *data, size_t len) {
    uint8_t blk[24], ks[32];
    uint64_t ctr = 0;
    memcpy(blk, nonce, 16);
    for (size_t off = 0; off < len; ctr++) {
        for (int j = 0; j < 8; j++) blk[16 + j] = (uint8_t)(ctr >> (56 - 8 * j));
        hmac_sha256(key, 32, blk, 24, ks);
        size_t n = (len - off < 32) ? len - off : 32;
        for (size_t j = 0; j < n; j++) data[off + j] ^= ks[j];
        off += n;
    }
    secure_zero(ks, sizeof ks);
}

int random_bytes(uint8_t *buf, size_t n) {
#ifdef _WIN32
    for (size_t i = 0; i < n;) {
        unsigned int r;
        if (rand_s(&r) != 0) return -1;
        for (int j = 0; j < 4 && i < n; j++, i++) buf[i] = (uint8_t)(r >> (8 * j));
    }
    return 0;
#else
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) return -1;
    size_t g = fread(buf, 1, n, f);
    fclose(f);
    return g == n ? 0 : -1;
#endif
}

int ct_equal(const uint8_t *a, const uint8_t *b, size_t n) {
    uint8_t d = 0;
    for (size_t i = 0; i < n; i++) d |= a[i] ^ b[i];
    return d == 0;
}

void secure_zero(void *p, size_t n) {
    volatile uint8_t *v = (volatile uint8_t *)p;
    while (n--) *v++ = 0;
}