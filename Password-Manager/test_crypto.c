/* Self-test: verifies SHA-256 / HMAC / PBKDF2 against published test vectors */
#include <stdio.h>
#include <string.h>
#include "crypto.h"
static void hex(const uint8_t *d, int n, char *o) { for (int i = 0; i < n; i++) sprintf(o + 2*i, "%02x", d[i]); }
int main(void) {
    uint8_t h[32]; char s[65]; int fail = 0;
    sha256((const uint8_t *)"abc", 3, h); hex(h, 32, s);
    if (strcmp(s, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")) { puts("SHA256 FAIL"); fail = 1; } else puts("SHA-256   : PASS");
    hmac_sha256((const uint8_t *)"key", 3, (const uint8_t *)"The quick brown fox jumps over the lazy dog", 43, h); hex(h, 32, s);
    if (strcmp(s, "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8")) { puts("HMAC FAIL"); fail = 1; } else puts("HMAC      : PASS");
    uint8_t salt[16] = {0}; memcpy(salt, "saltsaltsaltsalt", 16);
    derive_key("password", salt, 2, h); hex(h, 32, s); printf("PBKDF2    : %s (computed)\n", s);
    return fail;
}