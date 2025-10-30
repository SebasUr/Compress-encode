#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <ctype.h> 

#include "huffman.h"
#include "compress.h"

void compute_f_s(int f_s[], const unsigned char* buf, ssize_t nread) {
    for (size_t i = 0; i < nread; i++) {
        unsigned char byte = buf[i];   // Leer el valor del byte (0–255)
        f_s[byte]++;                  // Incrementar el contador.
    }
}

int main(void) {
    const char *path = "bible.txt";

    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    struct stat st;
    if (stat(path, &st) != 0) { perror("stat"); close(fd); return 1; }
    size_t size = (size_t)st.st_size;

    // Se aloja memoria
    // SI size es distinto de reserva size bytes, sino 1 para evitar malloc(0)
    unsigned char *buf = malloc(size ? size : 1);
    if (!buf) { perror("malloc"); close(fd); return 1; }

    // Escribe datos en el buffer. La variable dice cuantos bytes se copiaron.
    // Utiliza ssize_t como tipo para almacenar un número correspondiente a la cantidad de bits
    // File descriptor, puntero donde se copian los bytes leídos, número máximo de bites a leer.
    ssize_t nread = read(fd, buf, size);
    
    if (nread <= 0){ 
        perror("read"); 
        free(buf); 
        close(fd); 
        return 1; 
    }

    for (size_t i = 0; i < nread && i < 16; i++)
        printf("%02x ", buf[i]);
    printf("\nRead %zd bytes\n", nread);

    int f_s[256] = {0};
    compute_f_s(f_s, buf, nread);

    // reservar arreglo de nodos activos (punteros)
    huffmanNode **activeNodes = malloc(256 * sizeof(huffmanNode *));
    if (!activeNodes) {
        perror("malloc activeNodes");
        free(buf); close(fd);
        return 1;
    }

    // Construir árbol Huffman y obtener códigos
    Code codes[256];
    huffmanNode *root = huffmanAlgorithm(f_s, activeNodes, codes);
    if (!root) {
        fprintf(stderr, "NULL Returned: possible bad file.\n");
        free(activeNodes);
        free(buf);
        close(fd);
        return 1;
    }

    if (compressFile(buf, (size_t)nread, "bible.huf", codes) != 0) {
        fprintf(stderr, "Error writing compressed file.\n");
    } else {
        printf("Compression successful: bible.huf\n");
        
        // Probar descompresión
        if (decompressFile("bible.huf", "bible_decompressed.txt") != 0) {
            fprintf(stderr, "Error decompressing file.\n");
        } else {
            printf("Decompression successful: bible_decompressed.txt\n");
        }
    }

    freeHuffmanTree(root);
    free(activeNodes);

    free(buf);
    close(fd);
    return 0;
}

