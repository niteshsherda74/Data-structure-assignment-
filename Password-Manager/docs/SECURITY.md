# Security Design Notes

## Scope

The password manager stores credential records in an encrypted vault file. The cryptographic code is a learning implementation intended to demonstrate hashing, HMAC, key derivation, random nonces/salts, authenticated storage, and secure-memory practices.

It should **not** be presented as production-grade cryptographic software.

## Key Derivation

The master password is processed using PBKDF2-HMAC-SHA256 with:

- 100,000 iterations
- 16-byte random salt
- 32-byte derived output

The salt is stored in the vault header. The master password itself is not stored in the vault.

## Key Separation

The derived material is used as the input to two HMAC operations with different labels:

- enc → encryption key
- mac → authentication key

This keeps the encryption and authentication roles separate within the design.

## Encryption

Credential data is transformed using an HMAC-SHA256 based counter-mode keystream:

~~~text
keystream = HMAC-SHA256(encryption_key, nonce || counter)
ciphertext = plaintext XOR keystream
~~~

A fresh 16-byte nonce is generated whenever the vault is saved.

## Integrity Protection

The project uses an Encrypt-then-MAC layout:

~~~text
HMAC(mac_key, header || ciphertext)
~~~

When loading a vault, the HMAC is checked **before** the ciphertext is decrypted. The comparison is performed using the project's constant-time comparison routine.

If authentication fails, the program rejects the vault as having either:

- an incorrect master password, or
- modified/tampered contents.

## Secret Handling

The implementation includes:

- Hidden master-password input
- Password masking in list output
- OS-provided random number generation
- Explicit zeroing of selected sensitive buffers
- Temporary plaintext buffer clearing after encryption

## Important Limitations

This implementation uses custom cryptographic construction for educational purposes. Production software should use established, audited primitives and libraries rather than implementing cryptography from scratch.

Recommended production direction:

- AEAD: AES-256-GCM or ChaCha20-Poly1305
- Password KDF: Argon2
- Audited libraries: OpenSSL, libsodium, or equivalent
- Stronger memory and process isolation where appropriate

The application does not protect against a compromised operating system, keyloggers, malware, or memory inspection while the vault is unlocked.
