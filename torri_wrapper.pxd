cimport cython

cdef extern from 'torri.h':
	ctypedef struct string:
		pass
	ctypedef struct array:
		pass
	ctypedef array array_byte
	array_byte torri__encode_jpeg(string file_path)
