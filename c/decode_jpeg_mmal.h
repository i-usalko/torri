#ifndef DECODE_JPEG_MMAL_H
#define DECODE_JPEG_MMAL_H

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

//#include <interface/vcsm/user-vcsm.h>
#include "bcm_host.h"
#include "interface/mmal/mmal.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_connection.h"
#include "interface/mmal/mmal_queue.h"
#include "interface/vcos/vcos.h"

#include "decode_jpeg.h"

#define MAX_BUFFERS 2

static inline int warn(const char *file, int line, const char *fmt, ...)
{
   int errsv = errno;
   va_list va;
   va_start(va, fmt);
   fprintf(stderr, "WARN(%s:%d): ", file, line);
   vfprintf(stderr, fmt, va);
   va_end(va);
   errno = errsv;
   return 1;
}

#define CHECK_CONDITION(cond, ...) \
do { \
   if (cond) { \
      int errsv = errno; \
      fprintf(stderr, "ERROR(%s:%d) : ", \
         __FILE__, __LINE__); \
      errno = errsv; \
      fprintf(stderr,  __VA_ARGS__); \
      abort(); \
   } \
} while(0)
#define WARN_ON(cond, ...) \
   ((cond) ? warn(__FILE__, __LINE__, __VA_ARGS__) : 0)
#define ERRSTR strerror(errno)
#define CHECK_STATUS(status, ...) WARN_ON(status != MMAL_SUCCESS, __VA_ARGS__); \
   if (status != MMAL_SUCCESS) goto error;

//static FILE *source_file;

/* Macros abstracting the I/O, just to make the example code clearer */
// #define SOURCE_OPEN(uri) \
//    source_file = fopen(uri, "rb"); if (!source_file) goto error;
// #define SOURCE_READ_CODEC_CONFIG_DATA(bytes, size) \
//    size = fread(bytes, 1, size, source_file); rewind(source_file)
// #define SOURCE_READ_DATA_INTO_BUFFER(a) \
//    a->length = fread(a->data, 1, a->alloc_size - 128, source_file); \
//    a->offset = 0
// #define SOURCE_CLOSE() \
//    if (source_file) fclose(source_file)

/** Context for our application */
static struct CONTEXT_T {
   VCOS_SEMAPHORE_T semaphore;
   MMAL_QUEUE_T *queue;
   MMAL_STATUS_T status;
} context;

DECODING_RESULT_T* decode_jpeg_mmal(char *file_path, bool mmaped, bool debug_info);

#endif  // DECODE_JPEG_MMAL_H