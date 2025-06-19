//huffman.h

#ifndef HUFFMAN_H__
#define HUFFMAN_H__

/*
 * @brief for huffman function
 */


typedef struct HuffmanNode {
	unsigned char data;
	int freq;
	char code[100];
	struct HuffmanNode *left;
	struct HuffmanNode *right;
}HuffmanNode;

typedef struct MinHeap {
	int size;
	int capacity;
	int uniqueData;
	HuffmanNode **node_array;
}MinHeap;


HuffmanNode* build_huffman_tree(uint32_t freq[]);
void generate_huffman_code(HuffmanNode *root, char *code, int depth, char *code_table[]);
#endif  //HUFFMAN_H__
