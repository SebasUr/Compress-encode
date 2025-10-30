#include "compress.h"
#include "bitwriter.h"
#include "bitreader.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int compressFile(const unsigned char *input, size_t input_size, const char *output_path, const Code codes[256]) {
    int fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644); // fd es el file descriptor del archivo de salida
    if (fd < 0) {
        return -1;
    }

    // Encabezado mágico por estándar unix
    if (write(fd, "HUF1", 4) != 4) {
        close(fd);
        return -1;
    }

    // se guardan 8 bytes del tamaño original
    uint64_t original_size = (uint64_t)input_size;
    if (write(fd, &original_size, sizeof(uint64_t)) != sizeof(uint64_t)) {
        close(fd);
        return -1;
    }

    // se guardan 256 bytes con las longitudes de los códigos
    unsigned char lens[256];
    for (int i = 0; i < 256; i++) {
        lens[i] = (unsigned char)codes[i].length;
    }
    if (write(fd, lens, 256) != 256) {
        close(fd);
        return -1;
    }

    // ** para guardar la posición del byte de trailing bits.
    off_t trailing_pos = lseek(fd, 0, SEEK_CUR);
    unsigned char trailing_placeholder = 0;
    if (write(fd, &trailing_placeholder, 1) != 1) {
        close(fd);
        return -1;
    }

    BitWriter bw;
    bitWriterInit(&bw, fd);

    for (size_t i = 0; i < input_size; i++) {
        unsigned char byte = input[i]; // leemos byte del archivo original
        Code code = codes[byte]; // busca el codigo huffman
        
        // vamos escribiendo si los codigos están bien generados.
        if (code.length > 0) {
            bitWriterWrite(&bw, code.bits, (int)code.length);
        }
    }

    int trailing_bits = bitWriterFlush(&bw);

    if (lseek(fd, trailing_pos, SEEK_SET) < 0) {
        close(fd);
        return -1;
    }
    
    unsigned char trailing_byte = (unsigned char)trailing_bits;
    if (write(fd, &trailing_byte, 1) != 1) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// Estructura para decodificación
typedef struct {
    int symbol;
    uint64_t code;
    size_t length;
} CanonicalEntry;

static int compareEntries(const void *a, const void *b) {
    const CanonicalEntry *ea = (const CanonicalEntry *)a;
    const CanonicalEntry *eb = (const CanonicalEntry *)b;
    
    if (ea->length != eb->length) {
        return (int)ea->length - (int)eb->length;
    }
    return ea->symbol - eb->symbol;
}

static void reconstructCanonicalCodes(const unsigned char lens[256], Code codes[256]) {
    // Crear lista de símbolos con longitud > 0
    CanonicalEntry entries[256];
    int count = 0;
    
    for (int i = 0; i < 256; i++) {
        if (lens[i] > 0) {
            entries[count].symbol = i;
            entries[count].length = lens[i];
            count++;
        }
    }
    
    if (count == 0) return;
    
    // Ordenar por longitud, luego por símbolo
    qsort(entries, count, sizeof(CanonicalEntry), compareEntries);
    
    // Generar códigos canónicos
    uint64_t code = 0;
    size_t prev_len = entries[0].length;
    
    for (int i = 0; i < count; i++) {
        if (entries[i].length > prev_len) {
            code <<= (entries[i].length - prev_len);
            prev_len = entries[i].length;
        }
        
        entries[i].code = code;
        codes[entries[i].symbol].bits = code;
        codes[entries[i].symbol].length = entries[i].length;
        code++;
    }
}

int decompressFile(const char *input_path, const char *output_path) {
    int fd_in = open(input_path, O_RDONLY);
    if (fd_in < 0) {
        return -1;
    }
    
    // Leer y verificar encabezado mágico
    char magic[4];
    if (read(fd_in, magic, 4) != 4 || memcmp(magic, "HUF1", 4) != 0) {
        close(fd_in);
        return -1;
    }
    
    // Leer tamaño original
    uint64_t original_size;
    if (read(fd_in, &original_size, sizeof(uint64_t)) != sizeof(uint64_t)) {
        close(fd_in);
        return -1;
    }
    
    // Leer longitudes de códigos
    unsigned char lens[256];
    if (read(fd_in, lens, 256) != 256) {
        close(fd_in);
        return -1;
    }
    
    // Leer trailing_bits
    unsigned char trailing_bits;
    if (read(fd_in, &trailing_bits, 1) != 1) {
        close(fd_in);
        return -1;
    }
    
    // Reconstruir códigos canónicos
    Code codes[256];
    memset(codes, 0, sizeof(codes));
    reconstructCanonicalCodes(lens, codes);
    
    // Abrir archivo de salida
    int fd_out = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        close(fd_in);
        return -1;
    }
    
    // Inicializar BitReader
    BitReader br;
    bitReaderInit(&br, fd_in);
    
    // Decodificar datos
    uint64_t bytes_written = 0;
    uint64_t current_code = 0;
    int current_length = 0;
    
    while (bytes_written < original_size) {
        int bit = bitReaderReadBit(&br);
        if (bit < 0) {
            close(fd_in);
            close(fd_out);
            return -1;
        }
        
        // Construir código bit a bit
        current_code = (current_code << 1) | bit;
        current_length++;
        
        // Buscar coincidencia en los códigos
        int found = -1;
        for (int i = 0; i < 256; i++) {
            if (codes[i].length == (size_t)current_length && 
                codes[i].bits == current_code) {
                found = i;
                break;
            }
        }
        
        if (found >= 0) {
            // Código encontrado, escribir símbolo
            unsigned char symbol = (unsigned char)found;
            if (write(fd_out, &symbol, 1) != 1) {
                close(fd_in);
                close(fd_out);
                return -1;
            }
            
            bytes_written++;
            current_code = 0;
            current_length = 0;
        }
    }
    
    close(fd_in);
    close(fd_out);
    return 0;
}
