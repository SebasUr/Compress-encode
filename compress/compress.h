#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdint.h>
#include <stddef.h>
#include "huffman.h"

// Compress data using Huffman coding
// input: input data buffer
// input_size: size of input data
// output_path: path to write compressed file
// codes: Huffman codes for each byte (0-255)
// Returns: 0 on success, -1 on error
int compressFile(const unsigned char *input, size_t input_size, const char *output_path, const Code codes[256]);

// Decompress a Huffman-coded file
// input_path: path to compressed file
// output_path: path to write decompressed file
// Returns: 0 on success, -1 on error
int decompressFile(const char *input_path, const char *output_path);

#endif // COMPRESS_H
