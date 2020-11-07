#ifndef TORRI_HEADER
#define TORRI_HEADER

#include <stdbool.h>

typedef void* voidptr;
typedef unsigned char* byteptr;

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

typedef array array_byte;

extern array_byte torri__decode_jpeg(string file_path, bool use_mmal, bool use_mmap);
extern string torri__gencmd(string cmd);

#endif