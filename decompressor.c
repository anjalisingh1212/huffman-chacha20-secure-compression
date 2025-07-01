//decompressor.c

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
        
int decompress_file(const char *input_file, const char *output_file){
	unsigned char *packedData = NULL;
	unsigned long long packedDataSize = 0;
	
	if(decrypt_data(input_file, &packedData, &packedDataSize) == -1)
		return -1;
	//printHex(packedData, packedDataSize);
	
	int offset = 0;
	int N = (int)packedData[offset++];
	size_t freqSize = 256;
	uint32_t freq[256] = {0};
	unsigned char symbol;
	
	for(int i = 0; i < N; i++){
		symbol = packedData[offset++];
		memcpy(&freq[symbol], packedData+offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		printf("char = %c, freq = %d\n", symbol, freq[symbol]);
	}
	MinHeap* root = build_huffman_tree(freq);
	HuffmanNode* head = root->node_array[0];
	uint8_t last_valid_bits;
	memcpy(&last_valid_bits, packedData+offset, sizeof(uint8_t));
	printf("valid bits = %d\n", last_valid_bits);
	offset++;	
	unsigned char byte = '0';
	int bit = 0;
	unsigned char buffer[BUFFER_SIZE];
	int i = 0;
    int bits_read = 0;
	int shift_bit = 7;
	int valid_bits = 8;	
	FILE *fpO = fopen(output_file, "w");
        if(fpO == NULL){
		fprintf(stderr, "Error in opening file\n");
                return -1;
        }
	printf("offset = %d, packedDataSize = %lld\n", offset, packedDataSize);	
	printHex(packedData, packedDataSize);
	while(offset < packedDataSize-1){
		byte = packedData[offset++];
		printf("\n\tbyte = %02X\n",byte);
		printf("offset : %d\n",offset);
		valid_bits = (offset == packedDataSize - 1) ? last_valid_bits : 8;
		shift_bit = 7;
		bits_read = 0;	
		while(bits_read < valid_bits){
			bit = (byte >> shift_bit) & 1;
			printf("bit = %d\n", bit);
			head = (bit == 0) ? head->left : head->right;
			if (head == NULL) {
            			fprintf(stderr, "Error: Corrupted Huffman tree traversal.\n");
            			fclose(fpO);
            			return -1;
        		}	
			if(head->left == NULL && head->right == NULL){
				buffer[i++] = head->data;
				printf("buffer = %c\n", buffer[i-1]);
				head = root->node_array[0];
				printf("head reset = %c\n", head->data);	
				if(i >= BUFFER_SIZE){
					if(fwrite(buffer, 1, i, fpO) <= 0){
						fprintf(stderr, "Couldn't write to file.\n");
						fclose(fpO);
						return -1;
					}
				}
			}
			shift_bit--;	
			bits_read++;
			}
	}	

	if(i > 0){
		if(fwrite(buffer, 1, i, fpO) <= 0){
         	fprintf(stderr, "Couldn't write to file.\n");
            fclose(fpO);
			return -1;
        }
	}
	
	free_Minheap_Huffman(root->node_array[0]);
	free(root->node_array);
	free(root);
	free(packedData);
	fclose(fpO);
	printf("Decompression complete\n");
	return 0;
}
