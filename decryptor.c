// decryptor.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sodium.h>
#include "decryptor.h"
#include "utils.h"

int decrypt_data(const char *input_file, unsigned char **packedData, unsigned long long *packedDataSize)
{

	FILE *fpI = fopen(input_file, "r");
	if (!fpI)
		errExit("fopen for decryption");

	// Verify the magic header
	char magicHeader[MAGIC_HEADER_LEN + 1];
	if (fread(magicHeader, 1, MAGIC_HEADER_LEN, fpI) < MAGIC_HEADER_LEN)
	{
		fprintf(stderr, "Failed to read magic header\n");
		fclose(fpI);
		return -1;
	}
	magicHeader[3] = '\0';  // Null-terminate for safety
	if (memcmp(magicHeader, MAGIC_HEADER, MAGIC_HEADER_LEN) != 0)
	{
		fprintf(stderr, "Invalid file format. Magic header not mismatch.\n");
		fclose(fpI);
		return -1;
	}

	//preare buffers for decryption
	unsigned long long cipherTextLen = 0;
	uint8_t salt[SALT_SIZE];
	uint8_t nonce[NONCE_SIZE];
	uint8_t key[crypto_aead_chacha20poly1305_ietf_KEYBYTES];
	
	// Get the password from the user
	char password[256];
	printf("Enter the password used for encryption:  ");
	if (fgets(password, sizeof(password), stdin) == NULL)
	{
		fprintf(stderr, "Failed to read password.\n");
		fclose(fpI);
		return -1;
	}
	password[strcspn(password, "\n")] = 0; // Remove trailing newline character

	// Initialize the Sodium library
	if (sodium_init() < 0)
	{
		fclose(fpI);
		errExit("libsodium initialization failed");
	}

	// Read the salt, nonce, and ciphertext length from the file
	fread(salt, 1, SALT_SIZE, fpI);
	fread(nonce, 1, NONCE_SIZE, fpI);
	fread(&cipherTextLen, 1, sizeof(cipherTextLen), fpI);

	// Derive key from password using salt
	if (crypto_pwhash(key, crypto_aead_chacha20poly1305_ietf_KEYBYTES, password, strlen(password), salt, OPSLIMIT, MEMLIMIT, crypto_pwhash_ALG_DEFAULT) < 0)
	{
		fclose(fpI);
		errExit("Key derivation failed (out of memory?)");
	}

	// Read the ciphertext from the file
	uint8_t cipherText[cipherTextLen];
	if (fread(cipherText, 1, cipherTextLen, fpI) < cipherTextLen)
	{
		fprintf(stderr, "Failed to read ciphertext from file\n");
		fclose(fpI);
		return -1;
	}

	// Allocate memory for the decrypted data
	*packedDataSize = cipherTextLen - crypto_aead_chacha20poly1305_ietf_ABYTES;
	uint8_t *tempDecrypted = malloc(*packedDataSize);
	if(!tempDecrypted)
	{
		fclose(fpI);
		errExit("Memory allocation failed for decrypted data");
	}

	// Decrypt the data
	if (crypto_aead_chacha20poly1305_ietf_decrypt(tempDecrypted, packedDataSize,
												  NULL,
												  cipherText, cipherTextLen,
												  NULL, 0,
												  nonce, key) != 0)
	{
		fprintf(stderr, "Decryption failed (invalid key, nonce, or ciphertext)\n");
		free(tempDecrypted);
		fclose(fpI);
		return -1;
	}

	*packedData = tempDecrypted;
	fclose(fpI);
	return 0;
}
