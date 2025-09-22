// decompressor.c
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "decompressor.h"
#include "utils.h"
#include "huffman.h"
#include "decryptor.h"

int decompress_file(const char *input_file, const char *output_file)
{
	printf("Proceeding for decompression. ..\n");
	
	unsigned char *packedData = NULL;
	unsigned long long packedDataSize = 0;

	// Decrypt and read the compressed data
	if (decrypt_data(input_file, &packedData, &packedDataSize) == -1)
		return -1;

	int offset = 0;
	int totalUniqueChar = (int)packedData[offset++];	// Number of unique symbols
	size_t freqSize = 256;
	uint32_t freq[256] = {0};
	unsigned char symbol;

	// Reconstruct frequency table
	for (int i = 0; i < totalUniqueChar; i++)
	{
		symbol = packedData[offset++];
		memcpy(&freq[symbol], packedData + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		//printf("char = %c, freq = %d\n", symbol, freq[symbol]);
	}

	// Rebuild Huffman tree
	MinHeap *root = build_huffman_tree(freq, totalUniqueChar);
	if(!root || root->size == 0)
	{
		free(packedData);
		errExit("Failed to build Huffman tree from frequency table");
	}
	HuffmanNode *head = root->node_array[0];
	if(!head)
	{
		free_Minheap_Huffman(root->node_array[0]);
		free(root->node_array);
		free(root);
		free(packedData);
		errExit("Huffman tree root is NULL");
	}
	// Get the number of valid bits in the last byte
	uint8_t last_valid_bits;
	memcpy(&last_valid_bits, packedData + offset, sizeof(uint8_t));
	offset++;
	
	FILE *fpO = fopen(output_file, "w");
	if (!fpO)
	{
		free_Minheap_Huffman(root->node_array[0]);	
		free(root->node_array);
		free(root);
		free(packedData);
		errExit("Fopen for output file");
	}

	// Special case: if the tree has only one node, write that character repeatedly
	if(head->left == NULL && head->right == NULL)
	{
		// Calculate the total number of valid bits in the encoded data.
		// (packedDataSize - offset - 1) gives the number of fully filled bytes (each with 8 bits).
		// The last byte is only partially filled, so we add 'last_valid_bits' separately.
		unsigned int totalBits = (packedDataSize - offset - 1)* 8 + last_valid_bits;
		
		for (int i = 0; i < totalBits; i++)
		{
			if (fputc(head->data, fpO) == EOF)
			{
				fprintf(stderr, "Couldn't write to output file.\n");
				fclose(fpO);
				free_Minheap_Huffman(root->node_array[0]);
				free(root->node_array);
				free(root);
				free(packedData);
				return -1;
			}
		}
		fclose(fpO);
		free_Minheap_Huffman(root->node_array[0]);
		free(root->node_array);
		free(root);
		free(packedData);
		return 0;
	}
	// Decode the packed data bit by bit and write to output file
	unsigned char byte = '0';
	unsigned char buffer[BUFFER_SIZE];
	int i = 0;

	while (offset < packedDataSize)
	{
		int valid_bits = (offset == packedDataSize - 1) ? last_valid_bits : 8;
		byte = packedData[offset++];
		int bits_read = 0;
		int shift_bit = 7;
		while (bits_read < valid_bits)
		{
			int bit = (byte >> shift_bit) & 1;
			head = (bit == 0) ? head->left : head->right;
			if (head == NULL)
			{
				fprintf(stderr, "Error: Corrupted Huffman tree traversal.\n");
				fclose(fpO);
				free_Minheap_Huffman(root->node_array[0]);
                free(root->node_array);
                free(root);
                free(packedData);
				return -1;
			}

			// Reached a leaf node — write symbol to buffer
			if (head->left == NULL && head->right == NULL)
			{
				buffer[i++] = head->data;
				head = root->node_array[0];
				if (i >= BUFFER_SIZE)
				{
					if (fwrite(buffer, 1, i, fpO) <= 0)
					{
						fprintf(stderr, "Couldn't write to output file.\n");
						fclose(fpO);
						free_Minheap_Huffman(root->node_array[0]);
                        free(root->node_array);
                        free(root);
                        free(packedData);
						return -1;
					}
					i = 0; // Reset buffer index after writing
				}
			}
			shift_bit--;
			bits_read++;
		}
	}

	// Write any remaining data in buffer
	if (i > 0)
	{
		if (fwrite(buffer, 1, i, fpO) <= 0)
		{
			fprintf(stderr, "Couldn't write to file.\n");
			fclose(fpO);
			free_Minheap_Huffman(root->node_array[0]);
			free(root->node_array);	
			free(root);
			free(packedData);
			return -1;
		}
	}

	// Clean up
	free_Minheap_Huffman(root->node_array[0]);
	free(root->node_array);
	free(root);
	free(packedData);
	fclose(fpO);
	return 0;
}