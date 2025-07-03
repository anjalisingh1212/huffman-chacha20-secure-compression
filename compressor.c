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

int compress_file(const char *input_file, const char *output_file)
{
	printf("Proceeding for compression...\n");
	uint32_t freq[256] = {0};  // Frequency table for all byte values
	size_t freqSize = 256;
	int totalUniqueChar = 0;
	unsigned char buffer[BUFFER_SIZE] = "\0";
	bool is_empty = true;

	// Open Input file for reading
	FILE *fpI = fopen(input_file, "rb");
	if (!fpI){
		errExit("Fopen input file");
	}

	// Count frequency of each byte in input file
	int bytesRead = 0;
	while (0 < (bytesRead = fread(buffer, 1, BUFFER_SIZE, fpI)))
	{
		for (int i = 0; i < bytesRead; i++)
		{
			if (freq[buffer[i]] == 0)
				totalUniqueChar++;
			freq[buffer[i]]++;
			is_empty = false;
		}
	}

	// Exit early if file is empty
	if (is_empty)
	{
		printf("Input file is empty. Nothing to compress.\n");
		fclose(fpI);
		return 0;
	}

	// Open output file for writing encrypted compressed data
	FILE *fpO = fopen(output_file, "wb");
	if (!fpO){
		errExit("Fopen input file");
	}

	// Build Huffman tree from frequency table
	MinHeap *heap = build_huffman_tree(freq);
	if (!heap)
	{
		fclose(fpI);
		fclose(fpO);
		fprintf(stderr,"Huffman tree build failed.\n");
		return -1;
	}

	char code[100];
	char *codeTable[freqSize]; // Array to hold Huffman codes for each byte
	memset(codeTable, 0, sizeof(codeTable));
	// Generate Huffman codes for each byte
	generate_huffman_code(heap->node_array[0], code, 0, codeTable);

	// Buffer for storing frequency + encoded bits
	size_t packedDataSize = 1;
	unsigned char *packedData = calloc(packedDataSize, sizeof(unsigned char));
	if (!packedData)
		errExit("Calloc packedData");
	
	// Save frequency table in packedData buffer
	int offset = save_frequency_table(&packedData, &packedDataSize, freq, freqSize, totalUniqueChar);

	// Encode file content and append to packedData
	encode_and_write_data(fpI, &packedData, &offset, &packedDataSize, codeTable);

	// Encrypt the packed buffer using ChaCha20-Poly1305
	uint8_t *cipherText = NULL;
	unsigned long long cipherTextLen = 0;
	uint8_t salt[SALT_SIZE] = {0};
	uint8_t nonce[NONCE_SIZE] = {0};
	if(encrypt_data(packedData, packedDataSize, &cipherText, &cipherTextLen, salt, nonce) != 0)
	{
		free(packedData);
		fclose(fpI);
		fclose(fpO);
		fprintf(stderr, "Encryption failed.\n");
		return -1;
	}

	// Write final output: header + salt + nonce + length + ciphertext
	fwrite(MAGIC_HEADER, 1, MAGIC_HEADER_LEN, fpO);
	fwrite(salt, 1, SALT_SIZE, fpO);
	fwrite(nonce, 1, NONCE_SIZE, fpO);
	fwrite(&cipherTextLen, 1, sizeof(unsigned long long), fpO);
	fwrite(cipherText, 1, cipherTextLen, fpO);

	// Free code_table after encoding is done
	for (int i = 0; i < freqSize; ++i)
	{
		if (codeTable[i])
		{
			free(codeTable[i]);
			codeTable[i] = NULL; // Safe cleanup
		}
	}

	// Free Huffman tree and buffers
	free_Minheap_Huffman(heap->node_array[0]);
	free(heap->node_array);
	free(heap);
	free(cipherText);
	free(packedData);
	fclose(fpO); // Close output file
	fclose(fpI); // Close input file

	return 0;
}

// Function to save the frequency table into a buffer
// Returns the offset after inserting the frequency table
int save_frequency_table(unsigned char **buffer, size_t *bufferSize, uint32_t *freq, size_t freqSize, int N)
{
	int offset = 0;
	*bufferSize = 1 + N * (1 + sizeof(int));
	void *temp = realloc(*buffer, *bufferSize);
	if (!temp)
	{
		errExit("Allocate memory failed for frequency table");
	}
	*buffer = (unsigned char *)temp;
	(*buffer)[offset++] = (unsigned char)N;

	for (int i = 0; i < freqSize; i++)
	{
		if (freq[i] > 0)
		{
			(*buffer)[offset] = (unsigned char)i;
			offset++;
			memcpy((*buffer) + offset, &freq[i], sizeof(uint32_t));
			offset += sizeof(uint32_t);
		}
	}
	return offset;
}

// Function to encode the file content into Huffman codes and append it to the buffer
void encode_and_write_data(FILE *fpI, unsigned char **buffer, int *offset, size_t *buffSize, char *code_table[])
{
	rewind(fpI); // Reset file pointer to the beginning of the input file

	unsigned char bitPackage = 0;
	unsigned char ch = 0;
	int bits_empty = 8;
	int total_bits = 0;
	*buffSize = (*buffSize) + BUFFER_GROWTH_CHUNK + 1; // +1 for the placeholder byte
	void *temp = realloc(*buffer, *buffSize);
	if (!temp)
	{
		errExit("Reallocation failed during encoding");
	}
	*buffer = (unsigned char *)temp;

	// Reserve one byte for storing number of valid bits in the last byte
	int placeholder = *offset;
	memcpy(*buffer + (*offset), &ch, sizeof(ch));
	(*offset)++;

	while (fread(&ch, sizeof(ch), 1, fpI))
	{
		if (!code_table[ch])
		{
			fprintf(stderr, "No Huffman code for byte 0x%02X ('%c')\n", ch, (ch >= 32 && ch < 127) ? ch : '?');
			return;
		}

		total_bits += strlen(code_table[ch]);
		for (int i = 0; i < strlen(code_table[ch]); i++)
		{
			int bit = (code_table[ch][i] == '1') ? 1 : 0;
			bits_empty--;
			int pos = bit << bits_empty;
			bitPackage = bitPackage | pos;
			if (bits_empty == 0)
			{
				if (*buffSize <= *offset + 1)
				{
					*buffSize += BUFFER_GROWTH_CHUNK;
					void *temp = realloc(*buffer, *buffSize);
					if (!temp)
					{
						errExit("Reallocation failed during encoding");
					}
					*buffer = (unsigned char *)temp;
				}
				(*buffer)[*offset] = bitPackage;
				bits_empty = 8;
				bitPackage = '\0';
				(*offset)++;
			}
		}
	}
	// If there are remaining bits, write the last partially filled byte
	if (bits_empty != 8)
	{
		if (*buffSize <= *offset + 1)
		{
			*buffSize += BUFFER_GROWTH_CHUNK;
			void *temp = realloc(*buffer, *buffSize);
			if (!temp)
			{
				errExit("Reallocation failed during encoding");
			}
			*buffer = (unsigned char *)temp;
		}
		(*buffer)[*offset] = bitPackage;
		(*offset)++;
	}

	// Store number of valid bits in the last byte at the placeholder
	uint8_t valid_bits = (bits_empty == 8) ? 8 : 8 - bits_empty;
	(*buffer)[placeholder] = valid_bits; // Store the number of valid bits in the placeholder byte
	*buffSize = *offset;				 // Update the buffer size to the current offset

	temp = realloc(*buffer, *buffSize);	 // Resize the buffer to the actual size used
	if (!temp)
		errExit("Reallocation failed during encoding");
	
	*buffer = (unsigned char *)temp;
}
