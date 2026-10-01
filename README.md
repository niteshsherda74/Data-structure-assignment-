# Encrypted Password Manager

> **Data Structures Mini Project — C**

A command-line password manager built in C that combines core data structures with an educational cryptographic storage layer. Credential entries are maintained using a **hash table**, **binary search tree (BST)**, and **stack**, while vault files are encrypted and integrity-protected before being written to disk.

## Project Overview

The project demonstrates how data structures can be combined in a practical application:

- **Hash table** — fast average-case lookup by website
- **Binary search tree** — alphabetical traversal and sorted listing
- **Stack** — undo the most recent deletion
- **Linked lists** — collision chains inside hash-table buckets
- **Cryptographic module** — SHA-256, HMAC-SHA256, PBKDF2-style key derivation, stream encryption, secure random bytes, and constant-time comparison

The project is intentionally designed as an **educational implementation**. The cryptographic code demonstrates concepts and should not be treated as production password-manager security.

## Features

- Create a new encrypted vault
- Unlock an existing vault with a master password
- Add, search, update, and delete credentials
- List stored sites alphabetically
- Undo the most recent deletion
- Generate random passwords
- Check password strength
- Change the master password
- Detect incorrect passwords and modified vault files
- Cross-platform hidden password input
- Crypto self-tests for SHA-256 and HMAC-SHA256

## Architecture

~~~text
User
  |
  v
CLI Interface (main.c)
  |
  +-- Password generation / strength checking
  |
  v
Vault Layer (vault.c / vault.h)
  +-- Hash Table ----> fast site lookup
  +-- BST -----------> sorted traversal
  +-- Stack ----------> undo deletion
  |
  v
Crypto Layer (crypto.c / crypto.h)
  +-- SHA-256
  +-- HMAC-SHA256
  +-- PBKDF2-style derivation
  +-- Stream encryption
  +-- Secure random generation
  |
  v
vault.pmv
(encrypted + integrity-protected)
~~~

## Data Structures

| Structure | Purpose | Typical complexity |
|---|---|---|
| Hash table with chaining | Add/search/update/delete by site | O(1) average lookup |
| Binary Search Tree | Alphabetical listing | O(h) insert/delete, O(n) traversal |
| Linked-list chains | Resolve hash collisions | O(chain length) |
| Stack | Undo last deletion (LIFO) | O(1) push/pop |

The hash table owns the credential entries, while BST nodes reference those same entries for sorted traversal.

## Security Design

1. A master password is processed with PBKDF2-HMAC-SHA256 using a random 16-byte salt and 100,000 iterations.
2. The derived 256-bit material is separated into encryption and MAC keys.
3. Credential data is encrypted using an HMAC-SHA256 counter-mode keystream with a fresh 16-byte nonce.
4. An HMAC is calculated over the file header and ciphertext.
5. The HMAC is verified before decryption using a constant-time comparison.
6. Sensitive buffers are explicitly zeroed where the implementation controls their lifetime.
7. Random values are obtained from the operating system RNG.

### Vault file layout

~~~text
"PMV1"
|
+-- iterations        4 bytes
+-- salt             16 bytes
+-- nonce            16 bytes
+-- ciphertext length 4 bytes
+-- ciphertext       variable
+-- HMAC tag         32 bytes
~~~

## Build

### GCC / MinGW / Clang

~~~bash
make
~~~

Or compile directly:

~~~bash
gcc -Wall -Wextra -O2 -o pwmanager main.c vault.c crypto.c
~~~

### Run

Linux / macOS:

~~~bash
./pwmanager
~~~

Windows:

~~~text
pwmanager.exe
~~~

Use a custom vault path if required:

~~~bash
./pwmanager myvault.pmv
~~~

## Crypto Self-Test

Compile and run:

~~~bash
make test_crypto
./test_crypto
~~~

The self-test checks SHA-256 and HMAC-SHA256 against known test vectors and computes a PBKDF2 test value.

## Repository Structure

~~~text
Data-structure-assignment-/
├── README.md
├── Makefile
├── main.c
├── vault.c
├── vault.h
├── crypto.c
├── crypto.h
├── test_crypto.c
├── docs/
│   ├── DATA_STRUCTURES.md
│   └── SECURITY.md
└── presentation/
    └── PasswordManager_Presentation.pptx
~~~

## Limitations & Future Scope

This is an academic/educational implementation, not a production password manager.

Potential future improvements:

- Replace the custom encryption construction with an audited AEAD such as AES-256-GCM or ChaCha20-Poly1305.
- Use a memory-hard password KDF such as Argon2.
- Replace the plain BST with a self-balancing tree such as an AVL tree.
- Add automated integration tests for the complete vault lifecycle.
- Add a GUI and safer clipboard handling.
- Add more robust crash-safe file handling and input validation.

The current project also cannot protect secrets from malware, keyloggers, or memory scraping while the vault is unlocked.

## Academic Scope

**Subject:** Data Structures  
**Language:** C  
**Project Type:** Mini Project  
**Core Concepts:** Hash Table, BST, Stack, Linked List, File Handling, Complexity Analysis

## Author

**Nitesh Pachar**

> This repository contains the implementation and documentation submitted as part of the Data Structures mini project.
