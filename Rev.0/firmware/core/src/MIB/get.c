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

extern const int64_t SSMP_ERROR_NONE;
extern const int64_t SSMP_ERROR_NO_SUCH_OBJECT;

int64_t MIB_get(const char *OID, value *v) {
    uint32_t hash = djb2(OID);

    int N = sizeof(OIDs) / sizeof(MIBItem);
    for (int i = 0; i < N; i++) {
        MIBItem item = OIDs[i];

        if ((hash == item.hash) && (strcmp(OID, item.OID) == 0) && (item.get != NULL)) {
            return item.get(v);
        }
    }

    v->tag = VALUE_UNKNOWN;

    return SSMP_ERROR_NO_SUCH_OBJECT;
}

int64_t MIB_get_sys_board_id(value *v) {
    char ID[32];
    int N = sys_id(ID, sizeof(ID));
    slice octets = {
        .capacity = 64,
        .length = N,
        .bytes = (char *)calloc(64, sizeof(uint8_t)),
    };

    memmove(octets.bytes, ID, N);

    v->tag = VALUE_OCTET_STRING;
    v->octets = octets;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_id(value *v) {
    v->tag = VALUE_UINT32;
    v->integer = CONTROLLER;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_version(value *v) {
    v->tag = VALUE_UINT16;
    v->integer = VERSION;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_released(value *v) {
    slice octets = {
        .capacity = 32,
        .length = 0,
        .bytes = (char *)calloc(32, sizeof(uint8_t)),
    };

    int N = snprintf(octets.bytes, octets.capacity, "%s", RELEASED);
    if (N > 0) {
        octets.length = N < strlen(RELEASED) ? N : strlen(RELEASED);
    }

    v->tag = VALUE_OCTET_STRING;
    v->octets = octets;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_datetime(value *v) {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    int N;

    if (!RTC_get_datetime(&year, &month, &day, &hour, &minute, &second)) {
        v->tag = VALUE_NULL;
    } else {
        slice octets = {
            .capacity = 32,
            .length = 0,
            .bytes = (char *)calloc(32, sizeof(uint8_t)),
        };

        N = snprintf(octets.bytes, octets.capacity, "%04u-%02u-%02u %02u:%02u:%02u", year, month, day, hour, minute, second);
        octets.length = N < 0 ? 0 : (N < 19 ? N : 19);

        v->tag = VALUE_OCTET_STRING;
        v->octets = octets;
    }

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_syserror(value *v) {
    v->tag = VALUE_UINT16;
    v->integer = (uint16_t)get_errors();

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_sysinfo(value *v) {
    v->tag = VALUE_UINT8;
    v->integer = 0;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_1_unlocked(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U4_get_relay(1);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_2_unlocked(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U4_get_relay(2);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_3_unlocked(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U4_get_relay(3);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_4_unlocked(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U4_get_relay(4);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_1_open(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U3_get_door(1);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_2_open(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U3_get_door(2);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_3_open(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U3_get_door(3);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_4_open(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U3_get_door(4);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_1_pushbutton(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U3_get_button(1);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_2_pushbutton(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U3_get_button(2);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_3_pushbutton(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U3_get_button(3);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_doors_4_pushbutton(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = U3_get_button(4);

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_alarms_tamper_detect(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = false;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_alarms_fire_alarm(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = false;

    return SSMP_ERROR_NONE;
}
