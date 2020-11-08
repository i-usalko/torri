cimport cython

cdef extern from 'torri.h':
	ctypedef bint bool
	ctypedef unsigned char * byteptr
	ctypedef void * voidptr
	struct string:
		void* str
		int len
		int is_lit

	struct array:
		int element_size
		void* data
		int len
		int cap

	struct Option_string:
		int ok
		int is_none
		string v_error
		int ecode
		void* data

	ctypedef array array_byte

	byteptr torri__decode_jpeg(string file_path, bool use_mmal, bool use_mmap)
	string torri__gencmd(string cmd)
