/* vault.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vault.h"
#include "crypto.h"

#define INIT_BUCKETS 16
#define KDF_ITERS 100000u
#define HDR_LEN 44                       /* magic4 + iters4 + salt16 + nonce16 + len4 */

/* ---------- helpers ---------- */
static int ci_cmp(const char *a, const char *b) {
    while (*a && *b) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d) return d;
        a++; b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

static size_t hash_site(const char *s, size_t n) {          /* djb2, case-insensitive */
    size_t h = 5381;
    for (; *s; s++) h = ((h << 5) + h) + (size_t)tolower((unsigned char)*s);
    return h % n;
}

static int valid_field(const char *s) {
    size_t n = strlen(s);
    if (n == 0 || n >= MAX_FIELD) return 0;
    return strpbrk(s, "\t\n\r") == NULL;
}

/* ---------- BST ---------- */
static BstNode *bst_insert(BstNode *r, Entry *e) {
    if (!r) {
        BstNode *n = malloc(sizeof *n);
        if (!n) return NULL;
        n->e = e; n->left = n->right = NULL;
        return n;
    }
    if (ci_cmp(e->site, r->e->site) < 0) r->left = bst_insert(r->left, e);
    else                                 r->right = bst_insert(r->right, e);
    return r;
}

static BstNode *bst_min(BstNode *r) { while (r->left) r = r->left; return r; }

static BstNode *bst_delete(BstNode *r, const char *site) {
    if (!r) return NULL;
    int c = ci_cmp(site, r->e->site);
    if (c < 0)      r->left = bst_delete(r->left, site);
    else if (c > 0) r->right = bst_delete(r->right, site);
    else {
        if (!r->left)  { BstNode *t = r->right; free(r); return t; }
        if (!r->right) { BstNode *t = r->left;  free(r); return t; }
        BstNode *m = bst_min(r->right);              /* two children: use successor */
        r->e = m->e;
        r->right = bst_delete(r->right, m->e->site);
    }
    return r;
}

static void bst_free(BstNode *r) {
    if (!r) return;
    bst_free(r->left); bst_free(r->right); free(r);
}

static void bst_walk(const BstNode *r, void (*fn)(const Entry *, void *), void *ctx) {
    if (!r) return;
    bst_walk(r->left, fn, ctx); fn(r->e, ctx); bst_walk(r->right, fn, ctx);
}

/* ---------- Vault (hash table) ---------- */
void vault_init(Vault *v) {
    v->nbuckets = INIT_BUCKETS; v->count = 0; v->root = NULL; v->undo = NULL;
    v->buckets = calloc(v->nbuckets, sizeof(Entry *));
}

void vault_free(Vault *v) {
    for (size_t i = 0; i < v->nbuckets; i++) {
        Entry *e = v->buckets[i];
        while (e) { Entry *n = e->next; secure_zero(e, sizeof *e); free(e); e = n; }
    }
    free(v->buckets);
    bst_free(v->root);
    while (v->undo) { UndoNode *n = v->undo->next; secure_zero(v->undo, sizeof *v->undo); free(v->undo); v->undo = n; }
    v->buckets = NULL; v->root = NULL; v->count = 0;
}

static void rehash(Vault *v) {
    size_t nn = v->nbuckets * 2;
    Entry **nb = calloc(nn, sizeof(Entry *));
    if (!nb) return;
    for (size_t i = 0; i < v->nbuckets; i++) {
        Entry *e = v->buckets[i];
        while (e) {
            Entry *next = e->next;
            size_t h = hash_site(e->site, nn);
            e->next = nb[h]; nb[h] = e;
            e = next;
        }
    }
    free(v->buckets);
    v->buckets = nb; v->nbuckets = nn;
}

Entry *vault_find(const Vault *v, const char *site) {
    for (Entry *e = v->buckets[hash_site(site, v->nbuckets)]; e; e = e->next)
        if (ci_cmp(e->site, site) == 0) return e;
    return NULL;
}

int vault_add(Vault *v, const char *site, const char *user, const char *pass) {
    if (!valid_field(site) || !valid_field(user) || !valid_field(pass)) return V_INVALID;
    if (vault_find(v, site)) return V_DUP;
    if (v->count + 1 > v->nbuckets * 3 / 4) rehash(v);
    Entry *e = calloc(1, sizeof *e);
    if (!e) return V_INVALID;
    strcpy(e->site, site); strcpy(e->user, user); strcpy(e->pass, pass);
    size_t h = hash_site(site, v->nbuckets);
    e->next = v->buckets[h]; v->buckets[h] = e;
    v->root = bst_insert(v->root, e);
    v->count++;
    return V_OK;
}

int vault_update(Vault *v, const char *site, const char *user, const char *pass) {
    Entry *e = vault_find(v, site);
    if (!e) return V_NOTFOUND;
    if (!valid_field(user) || !valid_field(pass)) return V_INVALID;
    strcpy(e->user, user); strcpy(e->pass, pass);
    return V_OK;
}

int vault_delete(Vault *v, const char *site) {
    size_t h = hash_site(site, v->nbuckets);
    Entry *prev = NULL, *e = v->buckets[h];
    while (e && ci_cmp(e->site, site) != 0) { prev = e; e = e->next; }
    if (!e) return V_NOTFOUND;
    UndoNode *u = malloc(sizeof *u);                   /* push to undo stack */
    if (u) {
        u->e = *e; u->e.next = NULL;
        u->next = v->undo; v->undo = u;
    }
    v->root = bst_delete(v->root, e->site);            /* remove from BST first */
    if (prev) prev->next = e->next; else v->buckets[h] = e->next;
    secure_zero(e, sizeof *e); free(e);
    v->count--;
    return V_OK;
}

int vault_undo(Vault *v) {
    if (!v->undo) return V_EMPTY;
    UndoNode *u = v->undo;
    v->undo = u->next;
    int r = vault_add(v, u->e.site, u->e.user, u->e.pass);
    secure_zero(u, sizeof *u); free(u);
    return r;
}

void vault_inorder(const Vault *v, void (*fn)(const Entry *, void *), void *ctx) {
    bst_walk(v->root, fn, ctx);
}

/* ---------- Serialization + encryption ---------- */
typedef struct { char *buf; size_t len; } Sink;

static void sink_add(const Entry *e, void *ctx) {
    Sink *s = ctx;
    s->len += sprintf(s->buf + s->len, "%s\t%s\t%s\n", e->site, e->user, e->pass);
}

static void put32(uint8_t *p, uint32_t x) {
    p[0] = x >> 24; p[1] = x >> 16; p[2] = x >> 8; p[3] = (uint8_t)x;
}
static uint32_t get32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

static void split_keys(const uint8_t master[32], uint8_t enc[32], uint8_t mac[32]) {
    hmac_sha256(master, 32, (const uint8_t *)"enc", 3, enc);   /* domain separation */
    hmac_sha256(master, 32, (const uint8_t *)"mac", 3, mac);
}

int vault_save(const Vault *v, const char *path, const char *master) {
    size_t cap = v->count * (3 * MAX_FIELD + 3) + 1;
    uint8_t *file = malloc(HDR_LEN + cap + 32);
    char *plain = malloc(cap);
    if (!file || !plain) { free(file); free(plain); return L_FILE; }
    Sink s = { plain, 0 };
    vault_inorder(v, sink_add, &s);

    uint8_t salt[16], nonce[16], mk[32], ek[32], ak[32];
    if (random_bytes(salt, 16) || random_bytes(nonce, 16)) { free(file); free(plain); return L_FILE; }
    memcpy(file, "PMV1", 4);
    put32(file + 4, KDF_ITERS);
    memcpy(file + 8, salt, 16);
    memcpy(file + 24, nonce, 16);
    put32(file + 40, (uint32_t)s.len);

    derive_key(master, salt, KDF_ITERS, mk);
    split_keys(mk, ek, ak);
    memcpy(file + HDR_LEN, plain, s.len);
    stream_crypt(ek, nonce, file + HDR_LEN, s.len);               /* encrypt */
    hmac_sha256(ak, 32, file, HDR_LEN + s.len, file + HDR_LEN + s.len); /* then MAC */

    secure_zero(plain, cap); free(plain);
    secure_zero(mk, 32); secure_zero(ek, 32); secure_zero(ak, 32);

    char tmp[512];
    snprintf(tmp, sizeof tmp, "%s.tmp", path);
    FILE *f = fopen(tmp, "wb");
    if (!f) { free(file); return L_FILE; }
    size_t total = HDR_LEN + s.len + 32;
    int ok = fwrite(file, 1, total, f) == total;
    fclose(f); free(file);
    if (!ok) return L_FILE;
    remove(path);
    return rename(tmp, path) == 0 ? L_OK : L_FILE;
}

int vault_load(Vault *v, const char *path, const char *master) {
    FILE *f = fopen(path, "rb");
    if (!f) return L_FILE;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz < HDR_LEN + 32) { fclose(f); return L_FORMAT; }
    uint8_t *d = malloc((size_t)sz);
    if (!d || fread(d, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(d); return L_FILE; }
    fclose(f);

    uint32_t iters = get32(d + 4), clen = get32(d + 40);
    if (memcmp(d, "PMV1", 4) || iters == 0 || iters > 10000000u ||
        (size_t)sz != HDR_LEN + (size_t)clen + 32) { free(d); return L_FORMAT; }

    uint8_t mk[32], ek[32], ak[32], mac[32];
    derive_key(master, d + 8, iters, mk);
    split_keys(mk, ek, ak);
    hmac_sha256(ak, 32, d, HDR_LEN + clen, mac);
    if (!ct_equal(mac, d + HDR_LEN + clen, 32)) {                 /* verify BEFORE decrypting */
        secure_zero(mk, 32); secure_zero(ek, 32); secure_zero(ak, 32); free(d);
        return L_AUTH;
    }
    stream_crypt(ek, d + 24, d + HDR_LEN, clen);
    secure_zero(mk, 32); secure_zero(ek, 32); secure_zero(ak, 32);

    char *p = (char *)d + HDR_LEN, *end = p + clen;
    int rc = L_OK;
    while (p < end) {
        char *nl = memchr(p, '\n', (size_t)(end - p));
        if (!nl) { rc = L_FORMAT; break; }
        *nl = 0;
        char *t1 = strchr(p, '\t'), *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
        if (!t1 || !t2) { rc = L_FORMAT; break; }
        *t1 = 0; *t2 = 0;
        if (vault_add(v, p, t1 + 1, t2 + 1) != V_OK) { rc = L_FORMAT; break; }
        p = nl + 1;
    }
    secure_zero(d, (size_t)sz); free(d);
    return rc;
}