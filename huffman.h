//huffman.h

#ifndef HUFFMAN_H__
#define HUFFMAN_H__

/**
 * @brief Node structure for Huffman tree.
 */
typedef struct HuffmanNode {
    unsigned char data;          // Character value (leaf nodes only)
    uint32_t freq;               // Frequency of the character
    char code[100];              // Huffman code (optional, for debugging/display)
    struct HuffmanNode *left;    // Left child
    struct HuffmanNode *right;   // Right child
} HuffmanNode;

/**
 * @brief Min-heap structure used for building the Huffman tree.
 */
typedef struct MinHeap {
    int size;                    // Current number of nodes
    HuffmanNode **node_array;   // Array of pointers to Huffman nodes
} MinHeap;

/**
 * @brief Builds the Huffman tree based on the character frequency table.
 * 
 * @param freq Array of 256 frequencies (uint32_t).
 * @return MinHeap* Pointer to the final min-heap containing the root node.
 */
MinHeap* build_huffman_tree(uint32_t freq[], int totalUniqueChar);

/**
 * @brief Creates a new Huffman node.
 * 
 * @param freq Frequency of the character.
 * @param data Character value (for leaf nodes).
 * @return HuffmanNode* Pointer to the newly created node.
 */
HuffmanNode *createNode(uint32_t freq, char data);

/**
 * @brief Extracts the minimum node from the min-heap.
 * 
 * @param heap Pointer to the min-heap.
 * @return HuffmanNode* Pointer to the extracted node.
 */
HuffmanNode* extractMin(MinHeap *heap);

/**
 * @brief Swaps two nodes in the min-heap array.
 * 
 * @param node_array Pointer to the array of Huffman nodes.
 * @param l Index of the first node.
 * @param r Index of the second node.
 */
void swap(struct HuffmanNode **node_array, int l, int r);

/**
 * @brief Restores the min-heap property by moving the last node up.
 * 
 * @param heap Pointer to the min-heap.
 */
void heapifyUp(MinHeap *heap);

/**
 * @brief Restores the min-heap property by moving the node down.
 * 
 * @param heap Pointer to the min-heap.
 */
void heapifyDown(MinHeap *heap);

/**
 * @brief Merges two Huffman nodes into a new parent node.
 * 
 * @param nodeA Pointer to the first node.
 * @param nodeB Pointer to the second node.
 * @return HuffmanNode* Pointer to the newly created parent node.
 */
HuffmanNode* mergeNodes(HuffmanNode *nodeA, HuffmanNode *nodeB);

/**
 * @brief Recursively generates Huffman codes and fills the code table.
 * 
 * @param root Pointer to the root of the Huffman tree.
 * @param code Current code string being built.
 * @param depth Current depth in the tree.
 * @param code_table Array to store the generated codes for each character.
 */
void generate_huffman_code(HuffmanNode *root, char *code, int depth, char *code_table[]);

/**
 * @brief Free the memory allocated for the Huffman tree
 * 
 * @param root Pointer to the root of the Huffman tree.
 */
void free_Minheap_Huffman(HuffmanNode *root);

#endif  //HUFFMAN_H__