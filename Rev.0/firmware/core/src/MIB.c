#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MIB.h>
#include <sys.h>

value MIB_get(const char *OID) {
    value v = {
        .tag = VALUE_UNKNOWN,
    };

    if (strcmp(OID, MIB_BOARD_ID) == 0) {
        char ID[32];
        int N = sys_id(ID, sizeof(ID));
        slice octets = {
            .capacity = 64,
            .length = N,
            .bytes = (char *)calloc(64, sizeof(uint8_t)),
        };

        memmove(octets.bytes, ID, N);

        v.tag = VALUE_OCTET_STRING;
        v.octets = octets;
    }

    if (strcmp(OID, MIB_CONTROLLER_ID) == 0) {
        v.tag = VALUE_UINT32;
        v.integer = CONTROLLER;
    }

    if (strcmp(OID, MIB_CONTROLLER_VERSION) == 0) {
        slice octets = {
            .capacity = 16,
            .length = 2,
            .bytes = (char *)calloc(16, sizeof(uint8_t)),
        };

        octets.bytes[0] = (VERSION >> 8) & 0x00ff;
        octets.bytes[1] = (VERSION >> 0) & 0x00ff;

        v.tag = VALUE_OCTET_STRING;
        v.octets = octets;
    }

    return v;
}