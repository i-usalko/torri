module torri

// VideoCore library includes
#flag -I @VROOT/thirdparty/vc

#include "interface/vmcs_host/vc_vchi_gencmd.h"
#include "interface/vmcs_host/vc_gencmd_defs.h"
#include "interface/vchi/vchi.h"

struct C.opaque_vchi_connection_api_t {
}

// v issue ? can't found this constant from imports
const(
	GENCMDSERVICE_MSGFIFO_SIZE = (4096 - 4)
)

fn C.vcos_init()
fn C.vchi_initialise(&C.vchi_instance) int
fn C.vchi_connect(&voidptr, u32, &C.vchi_instance) int
fn C.vc_vchi_gencmd_init(&C.vchi_instance, &voidptr, u32)
fn C.vc_gencmd_send(charptr, charptr) int
fn C.vc_gencmd_read_response(charptr, int) int
fn C.vc_gencmd_stop()
fn C.vchi_disconnect(&C.vchi_instance) int

/** ***************************************************************************
 * JPEG encoding/decoding
 */

pub fn encode_jpeg(file_path string) []byte {
	return 'Ok'.bytes()
}

/** ***************************************************************************
 * General commands for vchi
 */

pub fn gencmd(cmd string) ?string {
	C.vcos_init()
	mut vchi := [4096]byte{}  // Unknown size of VCHI_INSTANCE_T
	C.vchi_initialise(&vchi)
	mut connections := &voidptr(0)
	C.vchi_connect(connections, 0, vchi)
	mut vchi_connections := &C.opaque_vchi_connection_api_t{}
	C.vc_vchi_gencmd_init(&vchi, &vchi_connections, 1)
	mut buffer := [GENCMDSERVICE_MSGFIFO_SIZE]byte{}
	unsafe {
		C.memcpy(buffer, cmd.str, cmd.len)
	}
	C.vc_gencmd_send('%s', buffer)
	C.vc_gencmd_read_response(buffer, sizeof(buffer))
	C.vc_gencmd_stop()
	C.vchi_disconnect(&vchi)

	return cstring_to_vstring(buffer)
}

