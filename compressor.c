// compressor.c

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include "compressor.h"
#include "utils.h"
#include "huffman.h"
#include "encryptor.h"

	int compress_file(const char* input_file, const char* output_file){
		printf("Compression process starting\n");	
		uint32_t freq[256] = {0};
		size_t freqSize = 256;
		int totalUniqueChar = 0;
		unsigned char buffer[BUFFER_SIZE] = "\0";
		bool is_empty = true;
		int bytesRead = 0;

		//Open Input file
		FILE *fpI = fopen(input_file, "rb");
		if(fpI == NULL)
			errExit("Fopen");

		printf("Files opened successfully\n");
		while(0<(bytesRead = fread(buffer,1, BUFFER_SIZE, fpI)))
		{
			printf("bytes read = %d\n",bytesRead);
			for(int i = 0; i < bytesRead; i++)	{
				if(freq[buffer[i]] == 0)
					totalUniqueChar++;
				freq[buffer[i]]++;
				is_empty = false;
			}
		}

		if(is_empty){
			printf("Input file is empty. Nothing to compress.\n");
			fclose(fpI);
			return 0;
		}

		for(int i = 0; i < 256; i++){
			if(freq[i]>0){
				printf("%c, i = %d, freq = %d ",i,i,freq[i]);
			}
		}
		printf("\n");

		FILE *fpO = fopen(output_file, "w");
		if(fpO == NULL)
			errExit("Fopen");

		MinHeap* heap = build_huffman_tree(freq);
		if(!heap){
			printf("Tree build failed\n");
			return 0;
		}
		char code[100];
		char *codeTable[freqSize];
		memset(codeTable, 0, sizeof(codeTable));
		size_t packedDataSize = 1;
		unsigned char *packedData = calloc(packedDataSize,sizeof(unsigned char));
		if(!packedData){
			printf("Memory allocation failed\n");
			errExit("Allocate memory");
		}
		generate_huffman_code(heap->node_array[0], code, 0, codeTable); 
		
		int offset = save_frequency_table(&packedData, &packedDataSize, freq, freqSize, totalUniqueChar);

		encode_and_write_data(fpI, &packedData, &offset, &packedDataSize, codeTable);
			
		printf("Buffer (hex) so far: ");
		
		printHex(packedData, packedDataSize);	
		printf("packedDataSize = %ld\n",packedDataSize);
		uint8_t *cipherText = NULL;
		unsigned long long cipherTextLen = 0;
		uint8_t salt[SALT_SIZE] = {0};
		uint8_t nonce[NONCE_SIZE] = {0};
		encrypt_data(packedData, packedDataSize, &cipherText, &cipherTextLen, salt, nonce);
			
		fwrite(MAGIC_HEADER,1, MAGIC_HEADER_LEN, fpO);
		fwrite(salt, 1, SALT_SIZE, fpO);
		fwrite(nonce, 1, NONCE_SIZE, fpO);
		fwrite(&cipherTextLen, 1, sizeof(unsigned long long), fpO);
		fwrite(cipherText, 1, cipherTextLen, fpO);
		// Free code_table after encoding is done
		for (int i = 0; i < freqSize; ++i) {
			if (codeTable[i]) {
				printf("Freeing codeTable[%d] = %s\n", i, codeTable[i]);
				free(codeTable[i]);
				codeTable[i] = NULL; // Safe cleanup
			}
		}

	printf("heap size = %d\n", heap->size);	
	free_Minheap_Huffman(heap->node_array[0]);
	free(heap->node_array);
	free(heap);
	free(cipherText);
	free(packedData);
	fclose(fpO); //Close output file
	fclose(fpI); //Close input file
	
	return 0;
}
int save_frequency_table(unsigned char **buffer, size_t *bufferSize, int *freq, size_t freqSize, int N){
		int offset = 0;        
		*bufferSize = 1 + N*(1+sizeof(int));
		printf("buffer size = %ld\n", *bufferSize);
		void *temp = realloc(*buffer, *bufferSize);
		if(!temp){
			fprintf(stderr, "Memory allocation failed\n");
			errExit("Allocate memory");
		}
		*buffer = (unsigned char *)temp;
		(*buffer)[offset++] = (unsigned char)N;
		
		for(int i = 0; i < freqSize; i++){
			if(freq[i] > 0){
				//printf("i = %c, freq = %d, offset = %d\n", (char)i, freq[i], offset);
				(*buffer)[offset] = (unsigned char)i;
				offset++;
				memcpy((*buffer)+offset, &freq[i], sizeof(uint32_t));
				//printf("value inserted - %s\n",*buffer);
				offset += sizeof(uint32_t);
			}
		}
		return offset;
	}

	

	void encode_and_write_data(FILE *fpI, unsigned char **buffer, int *offset, size_t *buffSize, char *code_table[]){
		printf("\n\tencode data\n");
		unsigned char bitPackage = '\0';
		unsigned char ch = '0';
		int bits_empty = 8;
		char code[100] = {0};
		int total_bits = 0;
		*buffSize = (*buffSize) + BUFFER_GROWTH_CHUNK + 1; // +1 for the placeholder byte
		void *temp = realloc(*buffer, *buffSize);
		if(!temp){
			printf("Reallocation failed");
			errExit("Realloc");
		}
		*buffer = (unsigned char *)temp;
		int placeholder	= *offset;
		memcpy(*buffer+(*offset), &ch, sizeof(ch));
		printf("Left placeholder\n");
		(*offset)++;
		printf("offset = %d\n",*offset);	
		fseek(fpI, 0, SEEK_SET);
		while(fread(&ch, sizeof(ch), 1, fpI)){
			printf("read from file\n");
			if(ch >= 256){
				fprintf(stderr, "Bad byte value : %d\n",ch);
				return;
			}
			if (!code_table[ch]) {
				fprintf(stderr,"No Huffman code for byte 0x%02X ('%c')\n",ch,(ch >= 32 && ch < 127) ? ch : '?');
				return;
			}
			strcpy(code, code_table[ch]);
			printf("read buff = %c, code = %s\n",ch, code);
			total_bits += strlen(code);
			for(int i = 0; i < strlen(code); i++){
				int bit = (code[i] == '1') ? 1:0;
				bits_empty--;
				//printf("code i = %d, x = %d\n", code[i], bitPackage);
				int pos = bit << bits_empty;
			//	printf("pos = %d\n",pos);
				bitPackage = bitPackage | pos;	
				//printf("x after combining = %d\n", bitPackage);
			if(bits_empty == 0)
			{
				//printf("char written = %d\n", (unsigned char)bitPackage);
				if(*buffSize <= *offset+1){
					*buffSize += BUFFER_GROWTH_CHUNK;
					void *temp = realloc(*buffer, *buffSize);
					if(!temp){
						printf("Reallocation failed");
						errExit("Realloc");
					}
					*buffer = (unsigned char *)temp;
				}
				(*buffer)[*offset] = bitPackage;
				bits_empty = 8;
				bitPackage = '\0';
				(*offset)++;
			}
			}
		//	printf("bits empty = %d\n", bits_empty);
		}
		if(bits_empty != 8){
		//	printf("char written = %d\n", (unsigned char)bitPackage);
				if(*buffSize <= *offset+1){
					*buffSize += BUFFER_GROWTH_CHUNK;
					void *temp = realloc(*buffer, *buffSize);
					if(!temp){
						printf("Reallocation failed");
						errExit("Realloc");
					}
					*buffer = (unsigned char *)temp;
				}
				(*buffer)[*offset] = bitPackage;
				(*offset)++;
		}
		uint8_t valid_bits = (bits_empty == 8) ? 8 : 8-bits_empty;
		printf("valid bits %d\n",valid_bits);
		(*buffer)[placeholder] = valid_bits; // Store the number of valid bits in the placeholder byte
		*buffSize = *offset; // Update the buffer size to the current offset
		temp = realloc(*buffer, *buffSize); // Resize the buffer to the actual size used
		if(!temp){
			fprintf(stderr, "Memory reallocation failed\n");
			errExit("Realloc");	
		}
		*buffer = (unsigned char *)temp;
	}

	