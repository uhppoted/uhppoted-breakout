#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <crypt/hash/djb2.h>

#include <MIB.h>
#include <RTC.h>
#include <breakout.h>
#include <doors.h>
#include <sys.h>

#define LOGTAG "MIB"

extern const int64_t SSMP_ERROR_NONE;
extern const int64_t SSMP_ERROR_NO_SUCH_OBJECT;
extern const int64_t SSMP_ERROR_NO_ACCESS;

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

    if (!RTC_get_datetime(&year, &month, &day, &hour, &minute, &second, NULL)) {
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
    v->integer = (uint16_t)syserr_bitmask();

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_sysinfo(value *v) {
    v->tag = VALUE_UINT8;
    v->integer = 0;

    return SSMP_ERROR_NONE;
}

int64_t get_door_mode(uint8_t door, value *v) {
    uint8_t mode;

    if (doors_get_mode(door, &mode)) {
        v->tag = VALUE_UINT8;
        v->integer = mode;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t get_door_delay(uint8_t door, value *v) {
    uint8_t delay;

    if (doors_get_delay(door, &delay)) {
        v->tag = VALUE_UINT8;
        v->integer = delay;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t get_door_unlocked(uint8_t door, value *v) {
    bool unlocked;

    if (doors_get_unlocked(door, &unlocked)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = unlocked;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t get_door_open(uint8_t door, value *v) {
    bool open;

    if (doors_get_open(door, &open)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = open;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t get_door_pushbutton(uint8_t door, value *v) {
    bool pressed;

    if (doors_get_pushbutton(door, &pressed)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = pressed;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t MIB_get_door_1_mode(value *v) {
    return get_door_mode(1, v);
}

int64_t MIB_get_door_2_mode(value *v) {
    return get_door_mode(2, v);
}

int64_t MIB_get_door_3_mode(value *v) {
    return get_door_mode(3, v);
}

int64_t MIB_get_door_4_mode(value *v) {
    return get_door_mode(4, v);
}

int64_t MIB_get_door_1_delay(value *v) {
    return get_door_delay(1, v);
}

int64_t MIB_get_door_2_delay(value *v) {
    return get_door_delay(2, v);
}

int64_t MIB_get_door_3_delay(value *v) {
    return get_door_delay(3, v);
}

int64_t MIB_get_door_4_delay(value *v) {
    return get_door_delay(4, v);
}

int64_t MIB_get_door_1_unlocked(value *v) {
    return get_door_unlocked(1, v);
}

int64_t MIB_get_door_2_unlocked(value *v) {
    return get_door_unlocked(2, v);
}

int64_t MIB_get_door_3_unlocked(value *v) {
    return get_door_unlocked(3, v);
}

int64_t MIB_get_door_4_unlocked(value *v) {
    return get_door_unlocked(4, v);
}

int64_t MIB_get_door_1_open(value *v) {
    return get_door_open(1, v);
}

int64_t MIB_get_door_2_open(value *v) {
    return get_door_open(2, v);
}

int64_t MIB_get_door_3_open(value *v) {
    return get_door_open(3, v);
}

int64_t MIB_get_door_4_open(value *v) {
    return get_door_open(4, v);
}

int64_t MIB_get_door_1_pushbutton(value *v) {
    return get_door_pushbutton(1, v);
}

int64_t MIB_get_door_2_pushbutton(value *v) {
    return get_door_pushbutton(2, v);
}

int64_t MIB_get_door_3_pushbutton(value *v) {
    return get_door_pushbutton(3, v);
}

int64_t MIB_get_door_4_pushbutton(value *v) {
    return get_door_pushbutton(4, v);
}

int64_t MIB_get_alarm_tamper_detect(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = false;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_alarm_fire_alarm(value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = false;

    return SSMP_ERROR_NONE;
}
