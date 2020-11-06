cimport cython

from typing import AnyStr
from torri_wrapper cimport *
from libc.stdlib cimport free

cdef class Torri(object):

    def encode_jpeg(self, file_path: str) -> AnyStr:
        file_path_bytes = file_path.encode('UTF-8')
        cdef string v_string
        v_string.str = <void*>file_path_bytes
        v_string.len = len(file_path_bytes)
        result = torri__encode_jpeg(v_string)
        cdef bytes py_bytes_string
        try:
            py_bytes_string = (<char*>result.data)[:result.len]
            return py_bytes_string
        finally:
            free(result.data)

    def gencmd(self, cmd: str) -> str:
        cmd_bytes = str.encode('UTF-8')
        cdef string v_string
        v_string.str = <void*>cmd_bytes
        v_string.len = len(cmd_bytes)
        result = torri__gencmd(v_string)
        cdef bytes py_bytes_string
        if result.ok:
            try:
                py_bytes_string = (<char*>result.data)[:1]
                return py_bytes_string.decode('UTF-8')
            finally:
                free(result.data)
        return None
