#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <crypt/hash/djb2.h>

#include <MIB.h>
#include <RTC.h>
#include <U3.h>
#include <U4.h>
#include <breakout.h>
#include <sys.h>

#define LOGTAG "MIB"

extern const MIBItem OIDs[21];

value MIB_get(const char *OID) {
    uint32_t hash = djb2(OID);

    value v = {
        .tag = VALUE_UNKNOWN,
    };

    if (strcmp(OID, MIB_BOARD_ID.OID) == 0) {
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

    if (strcmp(OID, MIB_CONTROLLER_ID.OID) == 0) {
        v.tag = VALUE_UINT32;
        v.integer = CONTROLLER;
    }

    if (strcmp(OID, MIB_CONTROLLER_VERSION.OID) == 0) {
        v.tag = VALUE_UINT16;
        v.integer = VERSION;
    }

    if (strcmp(OID, MIB_CONTROLLER_RELEASED.OID) == 0) {
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

    if (strcmp(OID, MIB_CONTROLLER_DATETIME.OID) == 0) {
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

    if (strcmp(OID, MIB_CONTROLLER_SYSERROR.OID) == 0) {
        v.tag = VALUE_UINT8;
        v.integer = (uint8_t)get_errors();
    }

    if (strcmp(OID, MIB_CONTROLLER_SYSINFO.OID) == 0) {
        v.tag = VALUE_UINT8;
        v.integer = 0;
    }

    int N = sizeof(OIDs) / sizeof(MIBItem);
    for (int i = 0; i < N; i++) {
        MIBItem item = OIDs[i];

        if ((hash == item.hash) && (strcmp(OID, item.OID) == 0) && (item.get != NULL)) {
            return item.get();
        }
    }

    return v;
}

value MIB_get_doors_1_unlocked() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U4_get_relay(1)};
}

value MIB_get_doors_2_unlocked() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U4_get_relay(2)};
}

value MIB_get_doors_3_unlocked() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U4_get_relay(3)};
}

value MIB_get_doors_4_unlocked() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U4_get_relay(4)};
}

value MIB_get_doors_1_open() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U3_get_door(1)};
}

value MIB_get_doors_2_open() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U3_get_door(2)};
}

value MIB_get_doors_3_open() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U3_get_door(3)};
}

value MIB_get_doors_4_open() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U3_get_door(4)};
}

value MIB_get_doors_1_pushbutton() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U3_get_button(1)};
}

value MIB_get_doors_2_pushbutton() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U3_get_button(2)};
}

value MIB_get_doors_3_pushbutton() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U3_get_button(3)};
}

value MIB_get_doors_4_pushbutton() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = U3_get_button(4)};
}

value MIB_get_alarms_tamper_detect() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = false};
}

value MIB_get_alarms_fire_alarm() {
    return (value){.tag = VALUE_BOOLEAN, .boolean = false};
}
