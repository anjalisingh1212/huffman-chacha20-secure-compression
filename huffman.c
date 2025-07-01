// huffman.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "huffman.h"

MinHeap* build_huffman_tree(uint32_t *freq){
	printf("In huffman file\n");		
	MinHeap *heap = (MinHeap*)malloc(sizeof(MinHeap));
	heap->size = 0;
	heap->node_array = (HuffmanNode**)malloc(sizeof(HuffmanNode*));
	if(heap->node_array == NULL){
		printf("Memory allocation failed\n");
		return NULL;
	}
	for(int i = 0; i < 256; i++){
		if(freq[i] > 0){
    		heap->size++;
			heap->node_array = (HuffmanNode**)realloc(heap->node_array,sizeof(HuffmanNode*)*heap->size);
			if(heap->node_array == NULL){
				printf("Memory allocation failed\n");
				return NULL;
			}
			heap->node_array[heap->size-1] = createNode(freq[i], (char)i);
			//printf("inserted %c, %d\n", heap->node_array[heap->size-1]->data, heap->node_array[heap->size-1]->freq);
			heapifyUp(heap);
		}

	}
	printf("\nheap array\n");
	for(int i = 0; i < heap->size; i++){
		//printf("node = %p\n", heap->node_array[i]);
		printf("%d %c\n",heap->node_array[i]->freq, heap->node_array[i]->data);
	}
	printf("size of heap = %d\n", heap->size);
	while(heap->size > 1){
		HuffmanNode *nodeA = extractMin(heap);
		printf("nodeA = %d, %c\n", nodeA->freq, nodeA->data);	
		
		//printf("\nAfter heapify down\n");
		/*for(int i = 0; i < heap->size; i++){
			printf("%d %c\n",heap->node_array[i]->freq, heap->node_array[i]->data);
		}*/
		HuffmanNode *nodeB = extractMin(heap);
		printf("nodeB = %d, %c\n", nodeB->freq, nodeB->data);	
		
		//printf("i = %d, size = %d\n",i, heap->size);
		//printf("\nAfter 2nd heapify down\n");
		//for(int i = 0; i < heap->size; i++){
		//	printf("%d %c\n",heap->node_array[i]->freq, heap->node_array[i]->data);
		//}
		heap->node_array[heap->size] = mergeNodes(nodeA, nodeB);
		heap->size++;
		heapifyUp(heap);
		//printf("heapify up\n");
		//for(int i = 0; i < heap->size; i++){
		//	printf("%d %c\n",heap->node_array[i]->freq, heap->node_array[i]->data);
		//}
		
		//printf("i = %d, size = %d\n",i, heap->size);
	}

	printf("return from build tree\n");
	return heap;
}

HuffmanNode *createNode(int freq, char data){
	HuffmanNode *newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
	if(newNode == NULL){
		printf("Memory allocation failed\n");
		return NULL;
	}
	newNode->freq = freq;
	newNode->data = data;
	newNode->left = newNode->right = NULL;
	return newNode;
}

// Extracts the minimum node from the heap and restores the min-heap property
HuffmanNode* extractMin(MinHeap *heap){
	if(heap->size == 0){
		printf("Heap is empty\n");
		return NULL;
	}
	HuffmanNode *minNode = heap->node_array[0];
	//printf("minNode = %c, %d\n", minNode->data, minNode->freq);
	heap->node_array[0] = heap->node_array[heap->size-1];
	heap->node_array[heap->size-1] = NULL;
	heap->size--;
	heapifyDown(heap);
	//printf("After heapify down\n");
	/*for(int i = 0; i < heap->size; i++){
		printf("%d %c\n",heap->node_array[i]->freq, heap->node_array[i]->data);
	}*/
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
	//printf("Heapify\n");
	int i = heap->size-1;
	//printf("size = %d\n",i);
	while(i > 0){
		int p = (i-1)/2;
	//	printf("p = %d, i = %d\n",p, i);
		if(heap->node_array[p]->freq > heap->node_array[i]->freq){
	//		printf("swap\n");
			swap(heap->node_array, p, i);
			i = p;
		}
		else break;  
	}
}

void heapifyDown(MinHeap *heap){
	//printf("In heapify down");
	int par_idx = 0;
	int idx = 0;
	while(par_idx < heap->size){
		int left = (2*par_idx) + 1;
		int right = (2*par_idx) + 2;
		if(left >= heap->size && right >= heap->size){
		//	printf("break\n");
			break;
		}
		if(right >= heap->size || heap->node_array[left]->freq <= heap->node_array[right]->freq)
			idx = left;
		else	idx = right;
		//printf("swap i = %d, idx = %d\n",i, idx);
		
		//printf("Before swap %c - %c\n", heap->node_array[i]->data, heap->node_array[idx]->data);
		if(heap->node_array[par_idx]->freq > heap->node_array[idx]->freq)
		{
			//if parent less than child swap
			swap(heap->node_array, par_idx , idx);
		}
		else break;
		//	printf("After swap %c - %c\n", heap->node_array[i]->data, heap->node_array[idx]->data);
		par_idx = idx;
	}
}

//Merge the two smallest node from heap array and get anew node
HuffmanNode* mergeNodes(HuffmanNode *nodeA, HuffmanNode *nodeB){
	HuffmanNode *newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
	if (!newNode) {
    	fprintf(stderr, "Memory allocation failed in mergeNodes\n");
    	exit(EXIT_FAILURE);
	}

	newNode->freq = nodeA->freq + nodeB->freq;
	newNode->data = '\0';
	newNode->left = nodeA;
	newNode->right = nodeB;
	//printf("nodeA = %c, nodeB = %c\n",nodeA->data, nodeB->data);
	//printf("nodeA = %d, nodeB = %d\n",nodeA->freq, nodeB->freq);
	return newNode;

}

void generate_huffman_code(HuffmanNode *root, char *code, int depth, char *code_table[]){
        if(!root)
                return;
        printf("%d and %d\n", !root->left, !root->right);
        if(!root->left && !root->right)
        {
        printf("root = %c\n", root->data);
                code[depth] = '\0';
        printf("code = %s\n", code);
	if ((unsigned char)root->data < 256)
                code_table[(unsigned char)root->data] = strdup(code);
	else
		printf("Root data out of 256: %d\n", root->data);
        }
    code[depth] = '0';
	generate_huffman_code(root->left, code, depth+1, code_table);
    code[depth] = '1';
    generate_huffman_code(root->right, code, depth+1, code_table);

}

void free_Minheap_Huffman(HuffmanNode *root){
	if(root == NULL)
		return;

	free_Minheap_Huffman(root->left);
	free_Minheap_Huffman(root->right);

	free(root);
}
