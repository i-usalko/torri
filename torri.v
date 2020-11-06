module torri

// VideoCore library includes
#flag -I @VROOT/thirdparty/vc

#include "interface/vmcs_host/vc_vchi_gencmd.h"
#include "interface/vmcs_host/vc_gencmd_defs.h"
#include "interface/vchi/vchi.h"
#include "interface/vchi/connections/connection.h"

struct C.opaque_vchi_instance_handle_t {
}
struct C.opaque_vchi_connection_api_t {
}

// v issue ? can't found this constant from imports
const(
	GENCMDSERVICE_MSGFIFO_SIZE = (4096 - 4)
)

fn C.vcos_init()
fn C.vchi_initialise(voidptr) int
fn C.vchi_connect(&voidptr, u32, voidptr) int
fn C.vc_vchi_gencmd_init(voidptr, &voidptr, u32)
fn C.vc_gencmd_send(charptr, charptr) int
fn C.vc_gencmd_read_response(charptr, int) int
fn C.vc_gencmd_stop()
fn C.vchi_disconnect(voidptr) int

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
	C.vcos_init()
	mut vchi := [65535]byte{}  // Unknown size of VCHI_INSTANCE_T
	mut code := C.vchi_initialise(vchi)
	println('C.vchi_initialise(&vchi) : vchi is ${vchi}, return code is ${code}')
	mut vchi_connections := [65535]byte{}
	for i in 0..5 {
		code = C.vchi_connect(&byteptr(&vchi_connections), 0, &vchi)
		if code == 0 {
			break
		}
	}
	if code != 0 {
		println('VCHI connection failed : return code is ${code}')
		return 'VCHI connection failed : return code is ${code}'
	}
	println('C.vchi_connect(connections, 0, vchi) : return code is ${code}')
	C.vc_vchi_gencmd_init(&vchi, &byteptr(&vchi_connections), 1)
	mut buffer := [GENCMDSERVICE_MSGFIFO_SIZE]byte{}
	unsafe {
		C.memcpy(buffer, cmd.str, cmd.len)
	}
	code = C.vc_gencmd_send('%s', buffer)
	if code != 0 {
		println('vc_gencmd_send return not zero code: ${code}')
		return 'vc_gencmd_send return not zero code: ${code}'
	}
	println('C.vc_gencmd_send(\'%s\', buffer) : return code is ${code}')
	code = C.vc_gencmd_read_response(buffer, sizeof(buffer))
	if code != 0 {
		println('vc_gencmd_read_response return not zero code: ${code}')
		return 'vc_gencmd_read_response return not zero code: ${code}'
	}
	println('C.vc_gencmd_read_response(buffer, sizeof(buffer)) : return code is ${code}')
	C.vc_gencmd_stop()
	code = C.vchi_disconnect(byteptr(&vchi))
	println('C.vchi_disconnect(&vchi) : return code is ${code}')

	return string(buffer)
}

