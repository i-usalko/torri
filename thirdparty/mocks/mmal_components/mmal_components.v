module mmal_components

import dl

// VideoCore library includes
#flag -I @VROOT/../../vc

#include "interface/vmcs_host/vc_vchi_gencmd.h"
#include "interface/vmcs_host/vc_gencmd_defs.h"
#include "interface/vchi/vchi.h"

struct C.opaque_vchi_connection_api_t {
}

// v issue ? can't found this constant from imports
const(
	GENCMDSERVICE_MSGFIFO_SIZE = (4096 - 4)
)

/** ***************************************************************************
 * Dynamically linked library loading
 * /opt/vc/lib/libmmal_components.so
 */
pub fn init(libraries_folder string) bool {
	mut fixed_path := libraries_folder
	if fixed_path.len == 0 {
		fixed_path = '/opt/vc/lib/libmmal_components.so'
	} else {
		fixed_path = '${libraries_folder}/libmmal_components.so'
	}
	return dl.open(fixed_path, dl.rtld_now)
}
