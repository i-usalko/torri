module torri

// VideoCore library includes
#flag -I @VROOT/thirdparty/vc
#flag -I @VROOT/c
#flag @VROOT/c/gencmd.o
#flag @VROOT/c/decode_jpeg.o

#include "gencmd.h"
#include "decode_jpeg.h"

[typedef]
struct C.MMAL_BUFFER_HEADER_TYPE_SPECIFIC_T {
}

[typedef]
struct C.MMAL_BUFFER_HEADER_PRIVATE_T {
}

[typedef]
struct C.MMAL_BUFFER_HEADER_T {
	next &C.MMAL_BUFFER_HEADER_T			/**< Used to link several buffer headers together */
	priv &C.MMAL_BUFFER_HEADER_PRIVATE_T 	/**< Data private to the framework */

	cmd u32               					/**< Defines what the buffer header contains. This is a FourCC
                                   				with 0 as a special value meaning stream data */

	data byteptr           					/**< Pointer to the start of the payload buffer (should not be
                                   				changed by component) */
   	alloc_size u32       					/**< Allocated size in bytes of payload buffer */
	length u32            					/**< Number of bytes currently used in the payload buffer (starting
                                   				from offset) */
   	offset u32          					/**< Offset in bytes to the start of valid data in the payload buffer */

	flags u32            					/**< Flags describing properties of a buffer header (see
                                   				\ref bufferheaderflags "Buffer header flags") */

	pts i64               					/**< Presentation timestamp in microseconds. \ref MMAL_TIME_UNKNOWN
                                   				is used when the pts is unknown. */
	dts int64_t               				/**< Decode timestamp in microseconds (dts = pts, except in the case
												of video streams with B frames). \ref MMAL_TIME_UNKNOWN
                                   				is used when the dts is unknown. */

   t &MMAL_BUFFER_HEADER_TYPE_SPECIFIC_T 	/** Type specific data that's associated with a payload buffer */

   user_data voidptr          				/**< Field reserved for use by the client */

}

fn C._decode_jpeg(charptr) &C.MMAL_BUFFER_HEADER_T
fn C.send_command(charptr) charptr

/** ***************************************************************************
 * JPEG encoding/decoding
 */

pub fn decode_jpeg(file_path string) []byte {
	result := C._decode_jpeg(file_path.str)
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

