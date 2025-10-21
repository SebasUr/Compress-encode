#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stddef.h>
#include <stdint.h>

typedef struct huffmanNode {
    int symbol;              // Character/byte value (-1 for internal nodes)
    uint64_t weight;         // Frequency/weight
    unsigned long order;     // Order for tie-breaking
    struct huffmanNode* left;
    struct huffmanNode* right;
} huffmanNode;

// typedef struct Code {
//     unsigned char bits[32]; // Maximum 256 bits
//     size_t length;          // Length in bits
// } Code;

// Creates a new Huffman node
huffmanNode* createNode(int symbol, uint64_t weight, unsigned long order);

// Initializes the tree with leaf nodes from frequency array
// f_s: frequency array of size 256
// activeNodes: array to store created nodes (must be pre-allocated: malloc(256 * sizeof(huffmanNode*)))
// Returns: number of nodes created
size_t initializeTree(const int f_s[], huffmanNode* activeNodes[]);

// Comparator function for qsort
int nodeComparator(const void *a, const void *b);

// Builds the Huffman tree from frequency array
// f_s: frequency array of size 256
// activeNodes: working array (must be pre-allocated: malloc(256 * sizeof(huffmanNode*)))
// Returns: root node of the Huffman tree, or NULL on error
huffmanNode* huffmanAlgorithm(const int f_s[], huffmanNode* activeNodes[]);

// Frees the entire Huffman tree
void freeHuffmanTree(huffmanNode* root);

#endif // HUFFMAN_H