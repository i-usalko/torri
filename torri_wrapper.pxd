cimport cython

cdef extern from 'torri.h':
	ctypedef struct string:
		pass
	ctypedef struct array:
		pass
	ctypedef struct Option_string:
		pass
	ctypedef array array_byte
	ctypedef bool
	array_byte torri__encode_jpeg(string file_path)
	Option_string torri__gencmd(string cmd)
