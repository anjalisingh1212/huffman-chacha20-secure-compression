//Header for encryptor file

#ifndef ENCRYPTOR_H__
#define ENCRYPTOR_H__

#define OPSLIMIT crypto_pwhash_OPSLIMIT_INTERACTIVE
#define MEMLIMIT crypto_pwhash_MEMLIMIT_INTERACTIVE

#define SALT_SIZE 16
#define NONCE_SIZE 12

int encrypt_data(const unsigned char *buffer, const size_t buffSize,
                uint8_t **ciphertext, unsigned long long *cipherTextLen,
                uint8_t *salt, uint8_t *nonce);


#endif //ENCRYPTOR_H__
