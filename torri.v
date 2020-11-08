module torri

// VideoCore library includes
#flag -I @VROOT/thirdparty/vc
#flag -I @VROOT/c
#flag @VROOT/c/gencmd.o
#flag @VROOT/c/decode_jpeg_mmal.o
#flag @VROOT/c/decode_jpeg_18k.o

#include "gencmd.h"
#include "decode_jpeg.h"
#include "decode_jpeg_mmal.h"
#include "decode_jpeg_18k.h"

[typedef]
struct C.DECODING_RESULT_T {
	data voidptr
	length u32
	errors charptr
}

fn C.decode_jpeg_mmal(charptr, bool, bool) &C.DECODING_RESULT_T
fn C.decode_jpeg_18k(charptr, bool) &C.DECODING_RESULT_T
fn C.send_command(charptr) charptr

struct Blob {
mut:
	data byteptr
	length u32
	errors charptr
}

/** ***************************************************************************
 * JPEG encoding/decoding
 */

pub fn decode_jpeg(file_path string, use_mmal bool, use_mmap bool) byteptr {
	mut result := &C.DECODING_RESULT_T{}
	if use_mmal {
		result = C.decode_jpeg_mmal(file_path.str, use_mmap, false)
	} else {
		result = C.decode_jpeg_18k(file_path.str, use_mmap)
	}
	if isnil(result) {
		return 'Returned result is : empty'.bytes().data
	}
	if int(result.length) > 0 {
		return byteptr(result.data)
	}
	return 'Returned result is : empty'.bytes().data
}

/** ***************************************************************************
 * Read file with mmap
 */

pub fn read_file_with_mmap(file_path string) Blob {
	mut result := Blob{}
	result.data = 'Returned result is : empty'.bytes().data
	result.length = 26
	return result
}

/** ***************************************************************************
 * General commands for vchi
 */

pub fn gencmd(cmd string) string {
	result := C.send_command(cmd.str)
	if !isnil(result) {
		return cstring_to_vstring(result)
	}
	return 'Returned result is : empty'
}

