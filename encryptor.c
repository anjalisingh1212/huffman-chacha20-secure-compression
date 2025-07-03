// encryptor.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sodium.h>
#include "encryptor.h"
#include "utils.h"

int encrypt_data(const unsigned char *buffer, const size_t buffSize,
				 uint8_t **ciphertext, unsigned long long *cipherTextLen,
				 uint8_t *salt, uint8_t *nonce)
{

	uint8_t key[crypto_aead_chacha20poly1305_ietf_KEYBYTES];

	// Get the password from the user
	char password[256];
	printf("Enter a password for encryption: ");
	if (fgets(password, sizeof(password), stdin) == NULL)
	{
		fprintf(stderr, "Error reading password\n");
		return -1;
	}
	// Remove newline from fgets input, if present
	password[strcspn(password, "\n")] = 0;

	// Initialize the Sodium library
	if (sodium_init() < 0)
	{
		errExit("libsodium initialization failed");
	}
	// Generate random salt
	randombytes_buf(salt, SALT_SIZE);

	// Derive a 256-bit key from the password and salt
	if (crypto_pwhash(key, crypto_aead_chacha20poly1305_ietf_KEYBYTES, password, strlen(password), salt, OPSLIMIT, MEMLIMIT, crypto_pwhash_ALG_DEFAULT) < 0)
	{
		errExit("Key derivation failed (out of memory?)");
	}

	// Generate a random nonce (12 bytesvrequired)
	randombytes_buf(nonce, NONCE_SIZE);

	// Allocate memory for ciphertext: plaintext size + 16 bytes for MAC
	*cipherTextLen = buffSize + crypto_aead_chacha20poly1305_IETF_ABYTES;
	*ciphertext = malloc(*cipherTextLen);
	if (!*ciphertext)
	{
		errExit("Memory allocation failed for ciphertext");
	}
	
	// Perform authenticated encryption
	if (crypto_aead_chacha20poly1305_ietf_encrypt(
			*ciphertext, cipherTextLen,
			buffer, buffSize,
			NULL, 0, // no additional data
			NULL, nonce, key) != 0)
	{
		fprintf(stderr, "Ecryption failed (invalid key, nonce, or ciphertext)\n");
		return -1;
	}
	return 0;
}
