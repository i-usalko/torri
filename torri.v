module torri

// VideoCore library includes
#flag -I @VROOT/thirdparty/vc
#flag -I @VROOT/c
#flag @VROOT/c/gencmd.o

#include "gencmd.h"

fn C.send_command(charptr) charptr

/** ***************************************************************************
 * JPEG encoding/decoding
 */

pub fn encode_jpeg(file_path string) []byte {
	return '${file_path} But Ok.'.bytes()
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

