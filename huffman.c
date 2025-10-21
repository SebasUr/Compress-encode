#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "huffman.h"

huffmanNode* createNode(int symbol, uint64_t weight, unsigned long order) {
    huffmanNode* NewNode = malloc(sizeof *NewNode);
    if (NewNode == NULL) { return NULL; }

    NewNode->weight = weight;
    NewNode->symbol = symbol; 
    NewNode->left = NULL;
    NewNode->right = NULL;
    NewNode->order = order;

    return NewNode;
}

// const int para no modificar f_s
// Importante cuando se llame debe ser malloc(256 * sizeof(huffmanNode*))
size_t initializeTree(const int f_s[], huffmanNode* activeNodes[]) {
    size_t count = 0;
    unsigned long next_order = 0;
    for (int i = 0; i < 256; i++){
        if (f_s[i] > 0){
            huffmanNode *n = createNode(i, (uint64_t)f_s[i], next_order++);
            if (!n) {
                for (size_t k = 0; k < count; ++k) free(activeNodes[k]);
                return 0;
            }
            activeNodes[count++] = n;
        }
    }
    return count;
}

int nodeComparator(const void *a, const void *b) {
    const huffmanNode *nodeA = *(const huffmanNode **)a;
    const huffmanNode *nodeB = *(const huffmanNode **)b;

    // Comparar por peso
    if(nodeA->weight < nodeB->weight) return -1;
    if(nodeA->weight > nodeB->weight) return 1;

    // Comparar por orden en caso de empate en peso
    if(nodeA->order < nodeB->order) return -1;
    if(nodeA->order > nodeB->order) return 1;
    return 0;
}

huffmanNode* huffmanAlgorithm(const int f_s[], huffmanNode* activeNodes[]) {
    size_t count = initializeTree(f_s, activeNodes);

    if (count == 0) return NULL;
    if (count == 1) { return activeNodes[0];}

    qsort(activeNodes, count, sizeof(huffmanNode*), nodeComparator);

    unsigned long next_order = 256;
    while(count > 1){
        huffmanNode* firstA = activeNodes[0];
        huffmanNode* firstB = activeNodes[1];

        huffmanNode* p_node = createNode(-1, firstA->weight + firstB->weight, next_order++);
        if (!p_node) { return NULL; }

        p_node->left = firstA;
        p_node->right = firstB;

        activeNodes[0] = p_node;
        activeNodes[1] = activeNodes[count-1];
        count -=1;
        qsort(activeNodes, count, sizeof(huffmanNode*), nodeComparator);
    }

    huffmanNode* root = activeNodes[0];
    return root;
}

void freeHuffmanTree(huffmanNode* root) {
    if (root == NULL) {
        return;
    }
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}



