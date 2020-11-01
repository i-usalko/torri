cimport cython

from typing import AnyStr
from torri_wrapper cimport *

cdef class Torri(object):

    def encode_jpeg(self, file_path: str) -> AnyStr:
        file_path_bytes = file_path.encode('UTF-8')
        cdef string v_string
        v_string.str = file_path_bytes
        result = torri__encode_jpeg(v_string)
        return result.data
