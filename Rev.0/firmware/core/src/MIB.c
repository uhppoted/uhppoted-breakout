#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MIB.h>
#include <RTC.h>
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
        v.tag = VALUE_UINT16;
        v.integer = VERSION;
    }

    if (strcmp(OID, MIB_CONTROLLER_RELEASED) == 0) {
        slice octets = {
            .capacity = 32,
            .length = 0,
            .bytes = (char *)calloc(32, sizeof(uint8_t)),
        };

        int N = snprintf(octets.bytes, octets.capacity, "%s", RELEASED);
        if (N > 0) {
            octets.length = N < strlen(RELEASED) ? N : strlen(RELEASED);
        }

        v.tag = VALUE_OCTET_STRING;
        v.octets = octets;
    }

    if (strcmp(OID, MIB_CONTROLLER_DATETIME) == 0) {
        slice octets = {
            .capacity = 32,
            .length = 0,
            .bytes = (char *)calloc(32, sizeof(uint8_t)),
        };

        char date[16] = {0};
        char time[16] = {0};
        char datetime[32] = {0};

        RTC_get_date(date, sizeof(date));
        RTC_get_time(time, sizeof(time));

        snprintf(datetime, sizeof(datetime), "%s %s", date, time);

        int N = snprintf(octets.bytes, octets.capacity, "%s", datetime);
        if (N > 0) {
            octets.length = N < strlen(datetime) ? N : strlen(datetime);
        }

        v.tag = VALUE_OCTET_STRING;
        v.octets = octets;
    }

    return v;
}