#pragma once

#include <encoding/ssmp/ssmp.h>

#define MIB_BOARD_ID "0.1.3.6.1.4.1.65536.1.1"
#define MIB_CONTROLLER_ID "0.1.3.6.1.4.1.65536.2.1"
#define MIB_CONTROLLER_VERSION "0.1.3.6.1.4.1.65536.2.2"

value MIB_get(const char *OID);
