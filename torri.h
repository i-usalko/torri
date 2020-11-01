#ifndef TORRI_HEADER
#define TORRI_HEADER

typedef void* voidptr;
typedef unsigned char* byteptr;

typedef struct string string;
typedef struct array array;

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

typedef array array_byte;

extern array_byte torri__encode_jpeg(string file_path);

float cmax(float, float);
int chello();

#endif