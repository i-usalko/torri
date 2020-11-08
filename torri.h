#ifndef TORRI_HEADER
#define TORRI_HEADER

#include <stdbool.h>
#include <stdint.h>

typedef void* voidptr;
typedef unsigned char* byteptr;
typedef uint32_t u32;
typedef char* charptr;

typedef struct string string;
typedef struct array array;
typedef struct Option_string Option_string;

struct string {
	byteptr str;
	int len;
	int is_lit;
};

struct array {
	int element_size;
	voidptr data;
	int len;
	int cap;
};

struct Option_string {
	bool ok;
	bool is_none;
	string v_error;
	int ecode;
	byteptr data;
};

typedef struct torri__Blob {
	byteptr data;
	u32 length;
	charptr errors;
} torri__Blob;

typedef array array_byte;

extern byteptr torri__decode_jpeg(string file_path, bool use_mmal, bool use_mmap);
extern string torri__gencmd(string cmd);
extern torri__Blob torri__read_file_with_mmap(string file_path);

#endif