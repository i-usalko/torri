module torri

// VideoCore library includes
#flag -I @VROOT/thirdparty/vc

#include "interface/vmcs_host/vc_vchi_gencmd.h"
#include "interface/vmcs_host/vc_gencmd_defs.h"

struct C.vchi_instance {
}
struct C.vchi_connection {
}

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
	mut vchi := C.vchi_instance{}
	C.vchi_initialise(&vchi)
	mut connections := &voidptr(0)
	C.vchi_connect(connections, 0, vchi)
	mut vchi_connections := &C.vchi_connection{}
	C.vc_vchi_gencmd_init(&vchi, &vchi_connections, 1)
	mut buffer := byte[GENCMDSERVICE_MSGFIFO_SIZE]{cmd.str}
	C.vc_gencmd_send('%s'.str, buffer)
	C.vc_gencmd_read_response(buffer, sizeof(buffer))
	C.vc_gencmd_stop()
	C.vchi_disconnect(&vchi)

	return cstring_to_vstring(buffer)
}

