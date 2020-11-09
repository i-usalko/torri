module torri

// VideoCore library includes
#flag -I @VROOT/thirdparty/vc
#flag -I @VROOT/c
#flag @VROOT/c/gencmd.o
#flag @VROOT/c/decode_jpeg_mmal.o
#flag @VROOT/c/decode_jpeg_18k.o
#flag @VROOT/c/read_file_with_mmap.o

#include "gencmd.h"
#include "decode_jpeg.h"
#include "decode_jpeg_mmal.h"
#include "decode_jpeg_18k.h"
#include "read_file_with_mmap.h"

[typedef]
struct C.DECODING_RESULT_T {
	data voidptr
	length int
	errors charptr
}

[typedef]
struct C.READING_RESULT_T {
	data voidptr
	length int
	errors charptr
}

fn C.decode_jpeg_mmal(charptr, bool, bool) &C.DECODING_RESULT_T
fn C.decode_jpeg_18k(charptr, bool) &C.DECODING_RESULT_T
fn C.send_command(charptr) charptr
fn C.read_file(charptr) &C.READING_RESULT_T

struct Blob {
mut:
	data byteptr
	length int
	errors charptr
}

/** ***************************************************************************
 * JPEG encoding/decoding
 */

pub fn decode_jpeg(file_path string, use_mmal bool, use_mmap bool) Blob {
	mut blob := Blob{}
	mut result := &C.DECODING_RESULT_T{}
	if use_mmal {
		result = C.decode_jpeg_mmal(file_path.str, use_mmap, false)
	} else {
		result = C.decode_jpeg_18k(file_path.str, use_mmap)
	}
	if isnil(result) {
		blob.length = -1
		blob.errors = 'Returned result is : empty'.bytes().data
	}
	if int(result.length) > 0 {
		blob.data = byteptr(result.data)
	} else if int(result.length) < 0 {
		errors := cstring_to_vstring(result.errors)
		blob.errors = charptr(errors.str)
	}

	blob.length = result.length
	return blob
}

/** ***************************************************************************
 * Read file with mmap
 */

pub fn read_file_with_mmap(file_path string) Blob {
	mut blob := Blob{}
	mut result := C.read_file(file_path.str)
	if isnil(result) {
		blob.length = -1
		blob.errors = 'Returned result is : empty'.bytes().data
	}
	if int(result.length) > 0 {
		blob.data = byteptr(result.data)
	} else if int(result.length) < 0 {
		errors := cstring_to_vstring(result.errors)
		blob.errors = charptr(errors.str)
	}

	blob.length = result.length
	return blob
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

