cimport cython

cdef extern from 'torri.h':
	ctypedef bool
	ctypedef byteptr
	ctypedef voidptr
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

	array_byte torri__decode_jpeg(string file_path)
	string torri__gencmd(string cmd)
