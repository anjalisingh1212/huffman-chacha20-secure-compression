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
	HuffmanNode **node_array;
}MinHeap;


MinHeap* build_huffman_tree(uint32_t freq[]);
HuffmanNode *createNode(int freq, char data);
HuffmanNode* extractMin(MinHeap *heap);
void swap(struct HuffmanNode **node_array, int l, int r);
void heapifyUp(MinHeap *heap);
void heapifyDown(MinHeap *heap);
HuffmanNode* mergeNodes(HuffmanNode *nodeA, HuffmanNode *nodeB);
void generate_huffman_code(HuffmanNode *root, char *code, int depth, char *code_table[]);
void free_Minheap_Huffman(HuffmanNode *root);

#endif  //HUFFMAN_H__
