cimport cython

from typing import AnyStr
from torri_wrapper cimport *

cdef class Torri(object):

    def encode_jpeg(self, file_path: str) -> AnyStr:
        file_path_bytes = file_path.encode('UTF-8')
        cdef string v_string
        v_string.str = <void*>file_path_bytes
        result = torri__encode_jpeg(v_string)
        #return <object>result.data
        return 'Ok'.encode('UTF-8')

    def gencmd(self, cmd: str) -> str:
        cmd_bytes = str.encode('UTF-8')
        cdef string v_string
        v_string.str = <void*>cmd_bytes
        result = torri__gencmd(v_string)
        if result.ok:
            return (<object>result.data).decode('UTF-8')
        return None
