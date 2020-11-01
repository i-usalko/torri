cimport cython

cdef extern from 'torri.h':
	ctypedef struct string
	ctypedef struct array
	ctypedef array array_byte
	array_byte torri__encode_jpeg(string file_path)
	float cmax(float, float)
	int chello()
