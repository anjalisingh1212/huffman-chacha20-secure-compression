// huffman.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "huffman.h"

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
	int size = heap->size;
	heap->node_array[0] = heap->node_array[size-1];
	//printf("first is %c, %d\n",heap->node_array[0]->data, heap->node_array[0]->freq);
	heap->size--;
	int i = 0;
	int idx = 0;
	while(i < heap->size){
		int left = (2*i) + 1;
		int right = (2*i) + 2;
		if(left >= heap->size && right >= heap->size){
		//`	printf("break\n");
			break;
		}
		if(right >= heap->size || heap->node_array[left]->freq <= heap->node_array[right]->freq)	idx = left;
		else	idx = right;
		//printf("swap i = %d, idx = %d\n",i, idx);
		
		//printf("Before swap %c - %c\n", heap->node_array[i]->data, heap->node_array[idx]->data);
		if(heap->node_array[i]->freq > heap->node_array[idx]->freq)
		{
			//if parent less than child swap
			swap(heap->node_array, i , idx);
		}
		else break;
		//	printf("After swap %c - %c\n", heap->node_array[i]->data, heap->node_array[idx]->data);
		i = idx;
	}
}

//Merge the two smallest node from heap array and get anew node
HuffmanNode* mergeNodes(HuffmanNode *nodeA, HuffmanNode *nodeB){
	HuffmanNode *newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
	newNode->freq = nodeA->freq + nodeB->freq;
	newNode->data = '\0';
	newNode->left = nodeA;
	newNode->right = nodeB;
	//printf("nodeA = %c, nodeB = %c\n",nodeA->data, nodeB->data);
	//printf("nodeA = %d, nodeB = %d\n",nodeA->freq, nodeB->freq);
	return newNode;

}

HuffmanNode* build_huffman_tree(uint32_t *freq){
	printf("In huffman file\n");	
	
	MinHeap *heap = (MinHeap*)malloc(sizeof(MinHeap));
	heap->size = 0;
	heap->capacity = 0;
	heap->uniqueData = 0;
	heap->node_array = (HuffmanNode**)malloc(sizeof(HuffmanNode*));
	if(heap->node_array == NULL){
		printf("Memory allocation failed\n");
		return NULL;
	}
	for(int i = 0; i < 256; i++){
		if(freq[i] > 0){
    			heap->size++;
			heap->capacity++;
			heap->uniqueData++;
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
	int i = 0;
	printf("size of heap = %d\n", heap->size);
	while(heap->size > 1){
		HuffmanNode *nodeA = heap->node_array[0];
		//printf("nodeA = %d, %c\n", nodeA->freq, nodeA->data);	
		heapifyDown(heap);
		//printf("\nAfter heapify down\n");
		/*for(int i = 0; i < heap->size; i++){
			printf("%d %c\n",heap->node_array[i]->freq, heap->node_array[i]->data);
		}*/
		HuffmanNode *nodeB = heap->node_array[0];
		//printf("nodeB = %d, %c\n", nodeB->freq, nodeB->data);	
		heapifyDown(heap);
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
		
		i++;
		//printf("i = %d, size = %d\n",i, heap->size);

	}
	printf("return from build tree\n");
	return heap->node_array[0];
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
                code_table[root->data] = strdup(code);
        }
        code[depth] = '0';
        generate_huffman_code(root->left, code, depth+1, code_table);
        code[depth] = '1';
        generate_huffman_code(root->right, code, depth+1, code_table);

}
