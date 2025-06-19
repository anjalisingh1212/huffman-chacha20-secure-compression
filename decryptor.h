//Header for decryptor file

#ifndef DECRYPTOR_H__
#define DECRYPTOR_H__

#define OPSLIMIT crypto_pwhash_OPSLIMIT_INTERACTIVE
#define MEMLIMIT crypto_pwhash_MEMLIMIT_INTERACTIVE

#define SALT_SIZE 16
#define NONCE_SIZE 12

int decrypt_data(const char *input_file, unsigned char **packedData, unsigned long long *packedDataSize);

#endif //DECRYPTOR_H__
