//Header for encryptor file

#ifndef ENCRYPTOR_H__
#define ENCRYPTOR_H__

// Key derivation parameters (tunable for performance vs. security)
#define OPSLIMIT crypto_pwhash_OPSLIMIT_INTERACTIVE
#define MEMLIMIT crypto_pwhash_MEMLIMIT_INTERACTIVE

// Encryption metadata sizes
#define SALT_SIZE 16
#define NONCE_SIZE 12

#include <stdint.h>

/**
 * @brief Encrypts input data using ChaCha20-Poly1305 AEAD cipher.
 *
 * @param buffer         Pointer to the plaintext data to encrypt.
 * @param buffSize       Size of the plaintext buffer.
 * @param ciphertext     Output pointer to hold allocated encrypted data (malloc'd inside).
 * @param cipherTextLen  Output variable for the length of encrypted data.
 * @param salt           Random salt used for key derivation (should be stored with output).
 * @param nonce          Random nonce used during encryption (should be stored with output).
 *
 * @return 0 on success, -1 on failure.
 */
int encrypt_data(const unsigned char *buffer, const size_t buffSize,
                uint8_t **ciphertext, unsigned long long *cipherTextLen,
                uint8_t *salt, uint8_t *nonce);


#endif //ENCRYPTOR_H__
