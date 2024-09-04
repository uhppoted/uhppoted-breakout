#include <stdio.h>
#include <string.h>

#include <MIB.h>

value MIB_get(const char *OID) {
    value v = {
        .tag = VALUE_UNKNOWN,
    };

    if (strcmp(OID, "0.1.3.6.655136.1.1") == 0) {
        v.tag = VALUE_UINT32;
        v.integer = 405419896;
    }

    return v;
}