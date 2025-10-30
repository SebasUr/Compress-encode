#include "bitreader.h"
#include <unistd.h>

void bitReaderInit(BitReader *br, int fd) {
    br->fd = fd;
    br->buffer = 0;
    br->pos = 0;
    br->bits_available = 0;
    br->eof = 0;
}

int bitReaderReadBit(BitReader *br) {
    // Si no hay bits disponibles, leer siguiente byte
    if (br->bits_available == 0) {
        ssize_t n = read(br->fd, &br->buffer, 1);
        if (n <= 0) {
            br->eof = 1;
            return -1;  // EOF o error
        }
        br->bits_available = 8;
        br->pos = 0;
    }

    // Leer bit más significativo disponible
    int bit = (br->buffer >> (7 - br->pos)) & 1;
    br->pos++;
    br->bits_available--;

    return bit;
}
