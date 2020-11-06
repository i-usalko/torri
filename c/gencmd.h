#ifndef GENCMD_H
#define GENCMD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "interface/vmcs_host/vc_vchi_gencmd.h"
#include "interface/vmcs_host/vc_gencmd_defs.h"

char* send_command(char *command);

#endif  // GENCMD_H