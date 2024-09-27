#include <stdio.h>
#include <string.h>

#include <MIB.h>

value MIB_get(const char *OID) {
    value v = {
        .tag = VALUE_UNKNOWN,
    };

    if (strcmp(OID, MIB_CONTROLLER_ID) == 0) {
        v.tag = VALUE_UINT32;
        v.integer = CONTROLLER;
    }

    return v;
}