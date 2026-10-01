/* main.c - Encrypted Password Manager (Data Structures mini project) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vault.h"
#include "crypto.h"

#ifdef _WIN32
#include <conio.h>
static void read_secret(const char *prompt, char *buf, size_t n) {
    size_t i = 0; int ch;
    printf("%s", prompt); fflush(stdout);
    while ((ch = _getch()) != '\r' && ch != '\n') {
        if (ch == '\b') { if (i) { i--; printf("\b \b"); } }
        else if (i < n - 1 && ch >= 32) { buf[i++] = (char)ch; putchar('*'); }
    }
    buf[i] = 0; putchar('\n');
}
#else
#include <termios.h>
#include <unistd.h>
static void read_secret(const char *prompt, char *buf, size_t n) {
    struct termios oldt, newt;
    int tty = isatty(STDIN_FILENO);
    printf("%s", prompt); fflush(stdout);
    if (tty) { tcgetattr(STDIN_FILENO, &oldt); newt = oldt; newt.c_lflag &= ~(tcflag_t)ECHO; tcsetattr(STDIN_FILENO, TCSANOW, &newt); }
    if (!fgets(buf, (int)n, stdin)) buf[0] = 0;
    if (tty) { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); putchar('\n'); }
    buf[strcspn(buf, "\r\n")] = 0;
}
#endif

static void read_line(const char *prompt, char *buf, size_t n) {
    printf("%s", prompt); fflush(stdout);
    if (!fgets(buf, (int)n, stdin)) buf[0] = 0;
    buf[strcspn(buf, "\r\n")] = 0;
}

/* ---------- password strength + generator ---------- */
static const char *COMMON[] = {"password","123456","12345678","qwerty","admin","letmein",
                               "welcome","iloveyou","abc123","monkey","dragon","111111", NULL};

static int strength(const char *p, const char **label) {
    int up = 0, lo = 0, di = 0, sy = 0, score = 0;
    size_t n = strlen(p);
    for (size_t i = 0; i < n; i++) {
        if (isupper((unsigned char)p[i])) up = 1;
        else if (islower((unsigned char)p[i])) lo = 1;
        else if (isdigit((unsigned char)p[i])) di = 1;
        else sy = 1;
    }
    for (int i = 0; COMMON[i]; i++) {
        char lower[MAX_FIELD]; size_t j;
        for (j = 0; p[j] && j < MAX_FIELD - 1; j++) lower[j] = (char)tolower((unsigned char)p[j]);
        lower[j] = 0;
        if (strstr(lower, COMMON[i])) { *label = "VERY WEAK (common word)"; return 0; }
    }
    score = up + lo + di + sy;
    if (n >= 12) score++;
    if (n < 8) score = score > 1 ? 1 : score;
    static const char *L[] = {"VERY WEAK", "WEAK", "FAIR", "GOOD", "STRONG", "EXCELLENT"};
    *label = L[score > 5 ? 5 : score];
    return score;
}

static void generate_password(char *out, int len) {
    static const char *sets[4] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz",
                                  "0123456789", "!@#$%^&*()-_=+?"};
    char all[128] = ""; 
    for (int i = 0; i < 4; i++) strcat(all, sets[i]);
    size_t an = strlen(all);
    for (;;) {
        int have[4] = {0};
        for (int i = 0; i < len; i++) {
            uint8_t b;
            do { random_bytes(&b, 1); } while (b >= 256 - (256 % an));   /* no modulo bias */
            out[i] = all[b % an];
            for (int s = 0; s < 4; s++) if (strchr(sets[s], out[i])) have[s] = 1;
        }
        out[len] = 0;
        if (len < 4 || (have[0] && have[1] && have[2] && have[3])) return;
    }
}

/* ---------- UI helpers ---------- */
static void print_row(const Entry *e, void *ctx) {
    int *i = ctx;
    printf("  %2d. %-24s %-24s ********\n", ++*i, e->site, e->user);
}

static void list_all(const Vault *v) {
    int i = 0;
    printf("\n  #   %-24s %-24s Password\n  -------------------------------------------------------------\n", "Site", "Username");
    vault_inorder(v, print_row, &i);
    if (!i) printf("  (vault is empty)\n");
    printf("\n");
}

static void save_or_warn(const Vault *v, const char *path, const char *master) {
    if (vault_save(v, path, master) != L_OK) printf("  [!] Could not save vault file!\n");
}

static void prompt_new_password(char *pass, const char *user_hint) {
    (void)user_hint;
    char choice[8];
    read_line("  Generate a strong password? (y/n): ", choice, sizeof choice);
    if (tolower((unsigned char)choice[0]) == 'y') {
        char lenb[16]; read_line("  Length (8-64, default 16): ", lenb, sizeof lenb);
        int len = atoi(lenb); if (len < 8 || len > 64) len = 16;
        generate_password(pass, len);
        printf("  Generated: %s\n", pass);
    } else {
        read_secret("  Password: ", pass, MAX_FIELD);
        const char *lab; strength(pass, &lab);
        printf("  Strength: %s\n", lab);
    }
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : "vault.pmv";
    char master[MAX_FIELD], confirm[MAX_FIELD], a[MAX_FIELD], b[MAX_FIELD], c[MAX_FIELD];
    Vault v; vault_init(&v);

    printf("\n=== Encrypted Password Manager (C | Hash Table + BST + Stack) ===\n");
    FILE *probe = fopen(path, "rb");
    if (probe) {
        fclose(probe);
        int ok = 0;
        for (int attempt = 0; attempt < 3 && !ok; attempt++) {
            read_secret("Master password: ", master, sizeof master);
            int r = vault_load(&v, path, master);
            if (r == L_OK) ok = 1;
            else if (r == L_AUTH) { printf("  [!] Wrong password or vault was tampered with.\n"); vault_free(&v); vault_init(&v); }
            else { printf("  [!] Vault file is corrupted.\n"); vault_free(&v); return 1; }
        }
        if (!ok) { printf("Too many attempts.\n"); vault_free(&v); return 1; }
        printf("Vault unlocked. %lu entries loaded.\n", (unsigned long)v.count);
    } else {
        printf("No vault found. Creating new vault '%s'.\n", path);
        for (;;) {
            read_secret("Set master password: ", master, sizeof master);
            read_secret("Confirm master password: ", confirm, sizeof confirm);
            if (strcmp(master, confirm)) { printf("  Passwords do not match.\n"); continue; }
            const char *lab;
            if (strlen(master) < 8 || strength(master, &lab) < 3) { printf("  Too weak (use 8+ chars with mixed types).\n"); continue; }
            break;
        }
        save_or_warn(&v, path, master);
    }

    for (;;) {
        printf("\n[1] Add   [2] Search   [3] List all (sorted)   [4] Update   [5] Delete\n"
               "[6] Undo delete   [7] Generate password   [8] Check strength\n"
               "[9] Change master password   [0] Save & exit\n> ");
        char ch[16]; read_line("", ch, sizeof ch);
        switch (atoi(ch)) {
        case 1: {
            read_line("  Site: ", a, sizeof a); read_line("  Username: ", b, sizeof b);
            prompt_new_password(c, b);
            int r = vault_add(&v, a, b, c);
            printf(r == V_OK ? "  Saved.\n" : r == V_DUP ? "  [!] Site already exists (use Update).\n" : "  [!] Invalid input (empty / too long / tab).\n");
            if (r == V_OK) save_or_warn(&v, path, master);
            break; }
        case 2: {
            read_line("  Site to search: ", a, sizeof a);
            Entry *e = vault_find(&v, a);
            if (!e) { printf("  Not found.\n"); break; }
            printf("  Site: %s\n  User: %s\n", e->site, e->user);
            read_line("  Reveal password? (y/n): ", b, sizeof b);
            printf("  Pass: %s\n", tolower((unsigned char)b[0]) == 'y' ? e->pass : "********");
            break; }
        case 3: list_all(&v); break;
        case 4: {
            read_line("  Site to update: ", a, sizeof a);
            if (!vault_find(&v, a)) { printf("  Not found.\n"); break; }
            read_line("  New username: ", b, sizeof b);
            prompt_new_password(c, b);
            printf(vault_update(&v, a, b, c) == V_OK ? "  Updated.\n" : "  [!] Invalid input.\n");
            save_or_warn(&v, path, master);
            break; }
        case 5:
            read_line("  Site to delete: ", a, sizeof a);
            if (vault_delete(&v, a) == V_OK) { printf("  Deleted (undo available).\n"); save_or_warn(&v, path, master); }
            else printf("  Not found.\n");
            break;
        case 6:
            if (vault_undo(&v) == V_OK) { printf("  Last deletion restored.\n"); save_or_warn(&v, path, master); }
            else printf("  Nothing to undo.\n");
            break;
        case 7: {
            read_line("  Length (8-64): ", a, sizeof a);
            int len = atoi(a); if (len < 8 || len > 64) len = 16;
            generate_password(c, len); printf("  %s\n", c); break; }
        case 8: {
            read_secret("  Password to test: ", a, sizeof a);
            const char *lab; int s = strength(a, &lab);
            printf("  Score %d/5 - %s\n", s, lab); break; }
        case 9:
            read_secret("  Current master: ", a, sizeof a);
            if (strcmp(a, master)) { printf("  [!] Incorrect.\n"); break; }
            read_secret("  New master: ", b, sizeof b);
            read_secret("  Confirm new master: ", c, sizeof c);
            const char *lab;
            if (strcmp(b, c) || strlen(b) < 8 || strength(b, &lab) < 3) { printf("  [!] Mismatch or too weak.\n"); break; }
            strcpy(master, b);
            save_or_warn(&v, path, master);
            printf("  Master password changed; vault re-encrypted with a new salt.\n");
            break;
        case 0:
            save_or_warn(&v, path, master);
            secure_zero(master, sizeof master); secure_zero(a, sizeof a);
            secure_zero(b, sizeof b); secure_zero(c, sizeof c); secure_zero(confirm, sizeof confirm);
            vault_free(&v);
            printf("Vault encrypted and saved. Goodbye!\n");
            return 0;
        default: printf("  Invalid choice.\n");
        }
    }
}