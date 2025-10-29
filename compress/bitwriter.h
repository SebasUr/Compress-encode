#ifndef BITWRITER_H
#define BITWRITER_H

#include <stdint.h>
#include <stddef.h>

typedef struct BitWriter {
    int fd;                  // descriptor
    unsigned char buffer;
    int bits_in_buffer;      // numero actual de bits en el buffer (0-7)
} BitWriter;

void bitWriterInit(BitWriter *bw, int fd);
void bitWriterWrite(BitWriter *bw, uint64_t bits, int count);
int bitWriterFlush(BitWriter *bw);

#endif // BITWRITER_H
