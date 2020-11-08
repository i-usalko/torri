cimport cython

from typing import AnyStr
from torri_wrapper cimport *
from libc.stdlib cimport free

cdef class Torri(object):

    def decode_jpeg(self, file_path: str, use_mmal: bool = True, use_mmap: bool = True) -> AnyStr:
        file_path_bytes = file_path.encode('UTF-8')
        cdef string _file_path
        _file_path.str = <char*>file_path_bytes
        _file_path.len = len(file_path_bytes)
        _file_path.is_lit = 0
        cdef bool _use_mmal = use_mmal
        cdef bool _use_mmap = use_mmap
        result = torri__decode_jpeg(_file_path, _use_mmal, _use_mmap)
        cdef bytes py_bytes_string
        py_bytes_string = memoryview(<unsigned char*>result)
        return py_bytes_string

    def gencmd(self, cmd: str) -> str:
        cmd_bytes = cmd.encode('UTF-8')
        cdef string v_string
        v_string.str = <char*>cmd_bytes
        v_string.len = len(cmd_bytes)
        v_string.is_lit = 0
        result = torri__gencmd(v_string)
        cdef bytes py_bytes_string
        try:
            py_bytes_string = (<char*>result.str)[:result.len]
            return py_bytes_string.decode('UTF-8')
        finally:
            free(<void*>result.str)

