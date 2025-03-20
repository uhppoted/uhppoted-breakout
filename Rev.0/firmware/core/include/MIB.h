#pragma once

#include <encoding/ssmp/ssmp.h>

#define MIB_BOARD_ID "0.1.3.6.1.4.1.65536.1.1"
#define MIB_CONTROLLER_ID "0.1.3.6.1.4.1.65536.2.1"
#define MIB_CONTROLLER_VERSION "0.1.3.6.1.4.1.65536.2.2"
#define MIB_CONTROLLER_RELEASED "0.1.3.6.1.4.1.65536.2.3"
#define MIB_CONTROLLER_DATETIME "0.1.3.6.1.4.1.65536.2.8"

value MIB_get(const char *OID);
int64_t MIB_set(const char *OID, const value u, value *v);