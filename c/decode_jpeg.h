#ifndef DECODE_JPEG_H
#define DECODE_JPEG_H

#include <stdint.h>

typedef struct DECODING_RESULT_T {
    void* data;
    uint32_t length;
    char* errors;
} DECODING_RESULT_T;


#endif  // DECODE_JPEG_H