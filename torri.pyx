cimport cython

from cython cimport view
from typing import AnyStr
from torri_wrapper cimport *
from libc.stdlib cimport free

cdef class Torri(object):

    def decode_jpeg(self, file_path: str, width: int, height: int, use_mmal: bool = True, use_mmap: bool = False) -> AnyStr:
        file_path_bytes = file_path.encode('UTF-8')
        cdef string _file_path
        _file_path.str = <char*>file_path_bytes
        _file_path.len = len(file_path_bytes)
        _file_path.is_lit = 0
        cdef bool _use_mmal = use_mmal
        cdef bool _use_mmap = use_mmap
        # Return gbr24 image
        cdef const unsigned char * result = torri__decode_jpeg(_file_path, _use_mmal, _use_mmap)
        cdef view.array mview = view.array(shape=(height*width*3,), itemsize=1, format='b', mode='c', allocate_buffer=False)
        mview.data = <char*> result
        mview.callback_free_data = free
        return mview

    def read_file_with_mmap(self, file_path: str) -> AnyStr:
        file_path_bytes = file_path.encode('UTF-8')
        cdef string _file_path
        _file_path.str = <char*>file_path_bytes
        _file_path.len = len(file_path_bytes)
        _file_path.is_lit = 0
        # Return blob
        cdef torri__Blob result = torri__read_file_with_mmap(_file_path)
        cdef unsigned int blob_length = result.length
        cdef view.array mview = view.array(shape=(blob_length,), itemsize=1, format='b', mode='c', allocate_buffer=False)
        mview.data = <char*> result.data
        mview.callback_free_data = free
        return mview

    def free(self, allocated_mview):
        cdef view.array as_view_array = <view.array>allocated_mview
        # Soft request
        del allocated_mview
        # print('Ok start free')
        # free(as_view_array.data)
        # print('Ok end free')

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

