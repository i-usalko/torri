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

fn C.decode_jpeg_mmal(charptr, bool) &C.DECODING_RESULT_T
fn C.decode_jpeg_18k(charptr, bool) &C.DECODING_RESULT_T
fn C.send_command(charptr) charptr

/** ***************************************************************************
 * JPEG encoding/decoding
 */

pub fn decode_jpeg(file_path string, use_mmal bool, use_mmap bool) []byte {
	mut result := &C.DECODING_RESULT_T{}
	if use_mmal {
		result = C.decode_jpeg_mmal(file_path.str, use_mmap)
	} else {
		result = C.decode_jpeg_18k(file_path.str, use_mmap)
	}
	println('torri-ok1')
	if isnil(result) {
		return 'Returned result is : empty'.bytes()
	}
	println('torri-ok2')
	if result.length > 0 {
		mut bytepile := []byte{}
		bytepile.insert_many(0, byteptr(result.data), int(result.length))
		return bytepile
	}
	println('torri-ok3')
	return 'Returned result is : empty'.bytes()
}

/** ***************************************************************************
 * General commands for vchi
 */

pub fn gencmd(cmd string) string {
	result := C.send_command(cmd.str)
	if !isnil(result) {
		return string(result)
	}
	return 'Returned result is : empty'
}

