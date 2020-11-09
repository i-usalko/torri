#ifndef READ_FILE_WITH_MMAP_H
#define READ_FILE_WITH_MMAP_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>

typedef struct READING_RESULT_T {
    void* data;
    int32_t length;
    char* errors;
} READING_RESULT_T;

READING_RESULT_T* read_file(char *file_path);

#endif  // READ_FILE_WITH_MMAP_H