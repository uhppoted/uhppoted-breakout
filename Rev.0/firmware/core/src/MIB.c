#include <stdio.h>
#include <string.h>

#include <MIB.h>

value MIB_get(const char *OID) {
    value v = {
        .tag = VALUE_UNKNOWN,
    };

    if (strcmp(OID, "0.1.3.6.1.4.1.65536.1.1") == 0) {
        v.tag = VALUE_UINT32;
        v.integer = CONTROLLER;
    }

    return v;
}