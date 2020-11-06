cimport cython

from typing import AnyStr
from torri_wrapper cimport *
from libc.stdlib cimport free

cdef class Torri(object):

    def encode_jpeg(self, file_path: str) -> AnyStr:
        file_path_bytes = file_path.encode('UTF-8')
        cdef string v_string
        v_string.str = <char*>file_path_bytes
        v_string.len = len(file_path_bytes)
        v_string.is_lit = 0
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
        v_string.str = <char*>cmd_bytes
        v_string.len = len(cmd_bytes)
        v_string.is_lit = 0
        result = torri__gencmd(v_string)
        cdef bytes py_bytes_string
        cdef string error = result.v_error
        if result.ok:
            try:
                py_bytes_string = (<char*>result.data)[:2]
                return py_bytes_string.decode('UTF-8')
            finally:
                free(result.data)
        else:
            try:
                py_bytes_string = (<char*>error.str)[:error.len]
                raise Exception(py_bytes_string.decode('UTF-8'))
            finally:
                free(error.str)
        return None
