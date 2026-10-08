/* vault.h - Data structures: Hash Table (chaining) + BST index + Undo Stack */
#ifndef VAULT_H
#define VAULT_H
#include <stddef.h>

#define MAX_FIELD 128

typedef struct Entry {              /* one saved credential (hash-chain node) */
    char site[MAX_FIELD], user[MAX_FIELD], pass[MAX_FIELD];
    struct Entry *next;
} Entry;

typedef struct BstNode {            /* BST index: keeps sites sorted A-Z */
    Entry *e;
    struct BstNode *left, *right;
} BstNode;

typedef struct UndoNode {           /* Stack of deleted entries */
    Entry e;
    struct UndoNode *next;
} UndoNode;

typedef struct {
    Entry   **buckets;              /* Hash table, O(1) average lookup */
    size_t    nbuckets, count;
    BstNode  *root;
    UndoNode *undo;                 /* top of stack */
} Vault;

/* return codes */
enum { V_OK = 0, V_DUP = -1, V_INVALID = -2, V_NOTFOUND = -3, V_EMPTY = -4 };
enum { L_OK = 0, L_FILE = -1, L_FORMAT = -2, L_AUTH = -3 };

void   vault_init(Vault *v);
void   vault_free(Vault *v);
int    vault_add(Vault *v, const char *site, const char *user, const char *pass);
Entry *vault_find(const Vault *v, const char *site);
int    vault_update(Vault *v, const char *site, const char *user, const char *pass);
int    vault_delete(Vault *v, const char *site);   /* pushes onto undo stack */
int    vault_undo(Vault *v);                       /* pops stack, restores entry */
void   vault_inorder(const Vault *v, void (*fn)(const Entry *, void *), void *ctx);

int    vault_save(const Vault *v, const char *path, const char *master);
int    vault_load(Vault *v, const char *path, const char *master);
#endif