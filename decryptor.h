//Header for decryptor file

#ifndef DECRYPTOR_H__
#define DECRYPTOR_H__

// Limits for password-based key derivation
#define OPSLIMIT crypto_pwhash_OPSLIMIT_INTERACTIVE
#define MEMLIMIT crypto_pwhash_MEMLIMIT_INTERACTIVE

// Encryption metadata sizes
#define SALT_SIZE 16
#define NONCE_SIZE 12

/**
 * @brief Decrypts data encrypted with ChaCha20-Poly1305 AEAD cipher.
 *
 * @param input_file      Path to the encrypted input file.
 * @param packedData      Output pointer to hold allocated decrypted buffer (malloc'd inside).
 * @param packedDataSize  Output variable to hold size of decrypted buffer.
 *
 * @return 0 on success, -1 on failure.
 */

int decrypt_data(const char *input_file, unsigned char **packedData, unsigned long long *packedDataSize);

#endif //DECRYPTOR_H__
