//decryptor.c used

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sodium.h>
#include <errno.h>
#include "decompressor.h"
#include "utils.h"
#include "decryptor.h"

int decrypt_data(const char *input_file, unsigned char **packedData, unsigned long long *packedDataSize){

	FILE *fpI = fopen(input_file, "r");
        if(fpI == NULL){
                fprintf(stderr, "Error in opening file\n");
		return -1;
	}
	char magicHeader[MAGIC_HEADER_LEN+1];
	if(fread(magicHeader, 1, MAGIC_HEADER_LEN, fpI) < MAGIC_HEADER_LEN){
		fprintf(stderr, "Problem in reading magic header\n");
		fclose(fpI);
		return -1;
	}
	magicHeader[3] = '\0';
	printf("magic header : %s\n",magicHeader);
	printf("Magic Header : %s\n",MAGIC_HEADER);
	if(memcmp(magicHeader, MAGIC_HEADER, MAGIC_HEADER_LEN) != 0){
		fprintf(stderr, "Invalid file format. Magic header not found.\n");
		fclose(fpI);
		return -1;
	}
	
	
	unsigned long long cipherTextLen = 0;
	uint8_t salt[SALT_SIZE];
	uint8_t nonce[NONCE_SIZE];
	uint8_t key[crypto_aead_chacha20poly1305_ietf_KEYBYTES];
	// Prompt user for password
        char password[256];
		char ch;
        while((ch = getchar()) != '\n' && ch != EOF);

        printf("Enter the encryption password used for decryption: ");
        
		if (fgets(password, sizeof(password), stdin) == NULL) {
    		fprintf(stderr, "Failed to read password.\n");
    		return -1;
		}

        // Remove newline from fgets input, if present
        password[strcspn(password, "\n")] = 0;
        // Initialize the Sodium library
	if(sodium_init() < 0){
		fprintf(stderr, "linsodium initialization failed\n");
		fclose(fpI);
		return -1;
	}

	fread(salt, 1, SALT_SIZE, fpI);
	fread(nonce, 1, NONCE_SIZE, fpI);
	fread(&cipherTextLen, 1, sizeof(cipherTextLen), fpI);
  //      printf("print salt\n");
    //    printHex(salt, SALT_SIZE);
     //   printf("print nonce");
      //  printHex(nonce, NONCE_SIZE);
	if(crypto_pwhash(key,  crypto_aead_chacha20poly1305_ietf_KEYBYTES, password, strlen(password), salt, OPSLIMIT, MEMLIMIT, crypto_pwhash_ALG_DEFAULT) < 0){
		fprintf(stderr, "Key derivation failed (out of memory?)\n");
		fclose(fpI);
		return -1;
	}
	
	uint8_t cipherText[cipherTextLen];
        if (fread(cipherText, 1, cipherTextLen, fpI) < cipherTextLen) {
                fprintf(stderr, "Failed to read ciphertext from file\n");
                fclose(fpI);
                return -1;
        }
//	printf("print cipher text\n");
        //printHex(cipherText, cipherTextLen);
	*packedDataSize = cipherTextLen - crypto_aead_chacha20poly1305_ietf_ABYTES;
	uint8_t *tempDecrypted = malloc(*packedDataSize);

	if(crypto_aead_chacha20poly1305_ietf_decrypt(tempDecrypted, packedDataSize,
					NULL,
					cipherText, cipherTextLen,
					NULL, 0,
					nonce, key) != 0){
		fprintf(stderr, "Decryption failed (invalid key, nonce, or ciphertext)\n");
		free(tempDecrypted);
		fclose(fpI);
        return -1;
	}
    
	*packedData = tempDecrypted;
	fclose(fpI);
	return 0;
}
