#ifndef READ_FILE_WITH_MMAP_H
#define READ_FILE_WITH_MMAP_H

#include <stdint.h>

typedef struct READING_RESULT_T {
    void* data;
    uint32_t length;
    char* errors;
} READING_RESULT_T;

READING_RESULT_T* read_file(char *file_path);

#endif  // READ_FILE_WITH_MMAP_H