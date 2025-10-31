#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stddef.h>

/**
 * Lee un archivo completo en memoria
 * 
 * MEMORIA: El caller debe liberar el buffer retornado con free()
 * 
 * @param path Ruta del archivo a leer
 * @param out_size Puntero donde se guardará el tamaño leído
 * @return Buffer con los datos del archivo, o NULL si error
 * 
 * Ejemplo:
 *   size_t size;
 *   unsigned char *data = readEntireFile("file.txt", &size);
 *   if (data) {
 *       // usar data...
 *       free(data);  // IMPORTANTE: liberar memoria
 *   }
 */
unsigned char* readEntireFile(const char *path, size_t *out_size);

/**
 * Crea directorios necesarios para una ruta de archivo
 * Similar a "mkdir -p" en Linux
 * 
 * @param path Ruta del archivo (ej: "dir1/dir2/file.txt")
 * @return 0 si éxito, -1 si error
 * 
 * Ejemplo:
 *   // Crea dir1/ y dir1/dir2/ si no existen
 *   ensureDirectoryExists("dir1/dir2/file.txt");
 */
int ensureDirectoryExists(const char *path);

#endif // FILE_UTILS_H
