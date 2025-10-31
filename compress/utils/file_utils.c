#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// Necesario para strdup en algunos sistemas
#define _POSIX_C_SOURCE 200809L
#include <libgen.h>

/**
 * Implementación de readEntireFile
 * Abre el archivo, lee todo su contenido en memoria y retorna un buffer
 */
unsigned char* readEntireFile(const char *path, size_t *out_size) {
    // Abrir archivo en modo solo lectura
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return NULL;
    }
    
    // Obtener tamaño del archivo con stat()
    struct stat st;
    if (fstat(fd, &st) != 0) {
        perror("fstat");
        close(fd);
        return NULL;
    }
    
    size_t size = (size_t)st.st_size;
    
    // Alojar memoria: si size es 0, alojar 1 byte para evitar malloc(0)
    unsigned char *buf = malloc(size ? size : 1);
    if (!buf) {
        perror("malloc");
        close(fd);
        return NULL;
    }
    
    // Leer todo el archivo en el buffer
    ssize_t nread = read(fd, buf, size);
    close(fd);
    
    // Verificar que se leyó correctamente
    if (nread < 0 || (size_t)nread != size) {
        perror("read");
        free(buf);
        return NULL;
    }
    
    *out_size = size;
    return buf;
}

/**
 * Crea todos los directorios padre necesarios para una ruta
 */
int ensureDirectoryExists(const char *path) {
    // Crear copia de la ruta porque dirname() puede modificarla
    size_t path_len = strlen(path);
    char *path_copy = (char *)malloc(path_len + 1);
    if (!path_copy) {
        perror("malloc");
        return -1;
    }
    strcpy(path_copy, path);
    
    // Obtener el directorio padre
    char *dir = dirname(path_copy);
    
    // Crear el directorio recursivamente
    // mkdir -p comportamiento: crear todos los padres necesarios
    char tmp[4096];
    char *p = NULL;
    size_t len;
    
    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    
    // Remover trailing slash si existe
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }
    
    // Crear directorios uno por uno
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            // Crear directorio, ignorar si ya existe (errno == EEXIST)
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    
    // Crear el último directorio
    mkdir(tmp, 0755);
    
    free(path_copy);
    return 0;
}
