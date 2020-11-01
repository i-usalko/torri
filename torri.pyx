cimport cython

from typing import AnyStr
from torri_wrapper cimport *

cdef class Torri(object):

    def encode_jpeg(self, file_path: str) -> AnyStr:
        #s = string(1)
        #r = ctorri.torri__encode_jpeg(s)
        #return r
        return cmax(1, 2)


# return 'Ok'.bytes()
