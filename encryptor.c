//Encrypt file

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sodium.h>
#include "encryptor.h"

int encrypt_data(const unsigned char *buffer, const size_t buffSize,
                uint8_t **ciphertext, unsigned long long *cipherTextLen,
                uint8_t *salt, uint8_t *nonce){
        
	uint8_t key[crypto_aead_chacha20poly1305_ietf_KEYBYTES];
	int ch;
	// Prompt user for password
	char password[256];
	while ((ch = getchar()) != '\n' && ch != EOF);
	printf("Enter a password for encryption: ");
	if (fgets(password, sizeof(password), stdin) == NULL) {
    	fprintf(stderr, "Error reading password\n");
    	return -1;
	}	
	// Remove newline from fgets input, if present
	password[strcspn(password, "\n")] = 0;
	

	printf("Password used for encryption is: %s\n", password);

	// Initialize the Sodium library
	if(sodium_init() < 0){
		fprintf(stderr, "libsodium initialization failed\n");
    		return -1;
	}
	// Generate random salt
	randombytes_buf(salt, SALT_SIZE);

	// Derive a 256-bit key from the password and salt
	if(crypto_pwhash(key, crypto_aead_chacha20poly1305_ietf_KEYBYTES, password, strlen(password), salt, OPSLIMIT, MEMLIMIT, crypto_pwhash_ALG_DEFAULT) < 0){
		fprintf(stderr, "Key derivation failed (out of memory?)\n");
		return -1;
	}

	// Generate a random nonce (12 bytes for IETF variant)
	randombytes_buf(nonce, NONCE_SIZE);
	
	// Allocate memory for ciphertext: plaintext size + 16 bytes for MAC
	*cipherTextLen = buffSize + crypto_aead_chacha20poly1305_IETF_ABYTES;
	*ciphertext = malloc(*cipherTextLen);
	// Perform authenticated encryption
	if(crypto_aead_chacha20poly1305_ietf_encrypt(
			*ciphertext, cipherTextLen, 
			buffer, buffSize, 
			NULL, 0,         // no additional data
		       	NULL, nonce, key) != 0)
	{
		fprintf(stderr, "Ecryption failed (invalid key, nonce, or ciphertext)\n");
        	return -1;
	}
	return 0;
}
