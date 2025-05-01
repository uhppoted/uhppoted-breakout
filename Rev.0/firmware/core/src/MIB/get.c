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
            return item.get(OID, v);
        }
    }

    v->tag = VALUE_UNKNOWN;

    return SSMP_ERROR_NO_SUCH_OBJECT;
}

int64_t MIB_get_sys_board_id(const char *OID, value *v) {
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

int64_t MIB_get_controller_id(const char *OID, value *v) {
    v->tag = VALUE_UINT32;
    v->integer = CONTROLLER;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_version(const char *OID, value *v) {
    v->tag = VALUE_UINT16;
    v->integer = VERSION;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_controller_released(const char *OID, value *v) {
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

int64_t MIB_get_controller_datetime(const char *OID, value *v) {
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

int64_t MIB_get_controller_syserror(const char *OID, value *v) {
    uint32_t hash = djb2(OID);

    if ((hash == MIB_CONTROLLER_SYSERROR.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR.OID) == 0)) {
        v->tag = VALUE_UINT16;
        v->integer = (uint16_t)syserr_bitmask();

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_MEMORY.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_MEMORY.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_MEMORY);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_I2C.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_I2C.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_I2C);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_QUEUE.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_QUEUE.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_QUEUE);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_RX8900SA.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_RX8900SA.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_RX8900SA);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_U2.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_U2.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_U2);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_U3.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_U3.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_U3);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_U4.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_U4.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_U4);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_WATCHDOG.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_WATCHDOG.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_WATCHDOG);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_DEBUG.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_DEBUG.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_DEBUG);

        return SSMP_ERROR_NONE;
    }

    if ((hash == MIB_CONTROLLER_SYSERROR_DEBUG.hash) && (strcmp(OID, MIB_CONTROLLER_SYSERROR_DEBUG.OID) == 0)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_DEBUG);

        return SSMP_ERROR_NONE;
    }

    v->tag = VALUE_UNKNOWN;

    return SSMP_ERROR_NO_SUCH_OBJECT;
}

int64_t MIB_get_controller_sysinfo(const char *OID, value *v) {
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

int64_t MIB_get_door_1_mode(const char *OID, value *v) {
    return get_door_mode(1, v);
}

int64_t MIB_get_door_2_mode(const char *OID, value *v) {
    return get_door_mode(2, v);
}

int64_t MIB_get_door_3_mode(const char *OID, value *v) {
    return get_door_mode(3, v);
}

int64_t MIB_get_door_4_mode(const char *OID, value *v) {
    return get_door_mode(4, v);
}

int64_t MIB_get_door_1_delay(const char *OID, value *v) {
    return get_door_delay(1, v);
}

int64_t MIB_get_door_2_delay(const char *OID, value *v) {
    return get_door_delay(2, v);
}

int64_t MIB_get_door_3_delay(const char *OID, value *v) {
    return get_door_delay(3, v);
}

int64_t MIB_get_door_4_delay(const char *OID, value *v) {
    return get_door_delay(4, v);
}

int64_t MIB_get_door_1_unlocked(const char *OID, value *v) {
    return get_door_unlocked(1, v);
}

int64_t MIB_get_door_2_unlocked(const char *OID, value *v) {
    return get_door_unlocked(2, v);
}

int64_t MIB_get_door_3_unlocked(const char *OID, value *v) {
    return get_door_unlocked(3, v);
}

int64_t MIB_get_door_4_unlocked(const char *OID, value *v) {
    return get_door_unlocked(4, v);
}

int64_t MIB_get_door_1_open(const char *OID, value *v) {
    return get_door_open(1, v);
}

int64_t MIB_get_door_2_open(const char *OID, value *v) {
    return get_door_open(2, v);
}

int64_t MIB_get_door_3_open(const char *OID, value *v) {
    return get_door_open(3, v);
}

int64_t MIB_get_door_4_open(const char *OID, value *v) {
    return get_door_open(4, v);
}

int64_t MIB_get_door_1_pushbutton(const char *OID, value *v) {
    return get_door_pushbutton(1, v);
}

int64_t MIB_get_door_2_pushbutton(const char *OID, value *v) {
    return get_door_pushbutton(2, v);
}

int64_t MIB_get_door_3_pushbutton(const char *OID, value *v) {
    return get_door_pushbutton(3, v);
}

int64_t MIB_get_door_4_pushbutton(const char *OID, value *v) {
    return get_door_pushbutton(4, v);
}

int64_t MIB_get_alarm_tamper_detect(const char *OID, value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = false;

    return SSMP_ERROR_NONE;
}

int64_t MIB_get_alarm_fire_alarm(const char *OID, value *v) {
    v->tag = VALUE_BOOLEAN;
    v->boolean = false;

    return SSMP_ERROR_NONE;
}
