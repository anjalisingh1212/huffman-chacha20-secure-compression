// huffman.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "huffman.h"
#include "utils.h"

// Builds a Huffman tree from the given frequency table
MinHeap* build_huffman_tree(uint32_t *freq, int totalUniqueChar){
	MinHeap *heap = (MinHeap*)malloc(sizeof(MinHeap));
	heap->size = 0;
	heap->node_array = (HuffmanNode**)malloc(sizeof(HuffmanNode*)*totalUniqueChar);
	if(heap->node_array == NULL)
		errExit("Memory allocation failed for Huffman node array");

	// Insert all non-zero frequency characters into the heap
	for(int i = 0; i < 256; i++){
		if(freq[i] > 0){
			heap->node_array[heap->size++] = createNode(freq[i], (char)i);
			heapifyUp(heap);
		}
	}
	
	// Merge nodes until only one tree remains
	while(heap->size > 1){
		HuffmanNode *nodeA = extractMin(heap);
		HuffmanNode *nodeB = extractMin(heap);
		
		heap->node_array[heap->size] = mergeNodes(nodeA, nodeB);
		heap->size++;
		heapifyUp(heap);
	}

	return heap;
}

// Creates a new Huffman node with the given frequency and character
HuffmanNode *createNode(uint32_t freq, char data){
	HuffmanNode *newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
	if(newNode == NULL)
		errExit("Memory allocation failed for new Huffman node");

	newNode->freq = freq;
	newNode->data = data;
	newNode->left = newNode->right = NULL;
	return newNode;
}

// Extracts the minimum node from the heap and restores the min-heap property
HuffmanNode* extractMin(MinHeap *heap){
	if(heap->size == 0)	return NULL;
	
	HuffmanNode *minNode = heap->node_array[0];
	heap->node_array[0] = heap->node_array[heap->size-1];
	heap->node_array[heap->size-1] = NULL;
	heap->size--;
	heapifyDown(heap);
	return minNode;
}

// Swaps two nodes in the heap array
void swap(struct HuffmanNode **node_array, int l, int r){
	HuffmanNode *temp = node_array[l];
	node_array[l] = node_array[r];
	node_array[r] = temp;
}

// Restores min-heap property by moving the last node up to its correct position
void heapifyUp(MinHeap *heap){
	int i = heap->size-1;
	while(i > 0){
		int p = (i-1)/2;
		if(heap->node_array[p]->freq > heap->node_array[i]->freq){
			swap(heap->node_array, p, i);
			i = p;
		}
		else break;  
	}
}

// Restores min-heap property by moving the node down to its correct position
void heapifyDown(MinHeap *heap){
	int par_idx = 0;
	int idx = 0;

	while(par_idx < heap->size){
		int left = (2*par_idx) + 1;
		int right = (2*par_idx) + 2;
		if(left >= heap->size && right >= heap->size)	break;
		
		if(right >= heap->size || heap->node_array[left]->freq <= heap->node_array[right]->freq)
			idx = left;
		else	idx = right;
		
		if(heap->node_array[par_idx]->freq > heap->node_array[idx]->freq)
			swap(heap->node_array, par_idx , idx);
		else break;

		par_idx = idx;
	}
}

// Merges two Huffman nodes into one new parent node
HuffmanNode* mergeNodes(HuffmanNode *nodeA, HuffmanNode *nodeB){
	HuffmanNode *newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
	if (!newNode) 
		errExit("Memory allocation failed for new Huffman node");

	newNode->freq = nodeA->freq + nodeB->freq;
	newNode->data = '\0'; // Internal node does not hold a character
	newNode->left = nodeA;
	newNode->right = nodeB;
	return newNode;
}

// Recursively generates Huffman codes and fills the code table
void generate_huffman_code(HuffmanNode *root, char *code, int depth, char *code_table[]){
    if(!root)	return;

	// Leaf node: assign the code string to the corresponding character
    if(!root->left && !root->right)
    {
		if(depth == 0){
			code[0] = '0'; // Assign '0' for the root node
        	code[1] = '\0';
		} else {
			code[depth] = '\0'; // Null-terminate the code string
		}
		code_table[(unsigned char)root->data] = strdup(code); // Store the code in the table
		return;
    }

	// Traverse left and right subtrees, appending '0' and '1' to the code
    code[depth] = '0';
	generate_huffman_code(root->left, code, depth+1, code_table);
    code[depth] = '1';
    generate_huffman_code(root->right, code, depth+1, code_table);

}

// Frees the memory allocated for the Huffman tree
void free_Minheap_Huffman(HuffmanNode *root){
	if(root == NULL)
		return;

	free_Minheap_Huffman(root->left);
	free_Minheap_Huffman(root->right);

	free(root);
}
