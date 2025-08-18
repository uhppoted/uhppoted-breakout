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

extern bool equal(const char *OID, const MIBItem oid);

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
    if (equal(OID, MIB_CONTROLLER_SYSERROR)) {
        v->tag = VALUE_UINT16;
        v->integer = (uint16_t)syserr_bitmask();

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_MEMORY)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_MEMORY);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_I2C)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_I2C);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_QUEUE)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_QUEUE);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_RX8900SA)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_RX8900SA);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_U2)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_U2);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_U3)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_U3);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_U4)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_U4);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_WATCHDOG)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_WATCHDOG);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_DEBUG)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = syserr_get(ERR_DEBUG);

        return SSMP_ERROR_NONE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR_DEBUG)) {
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

int64_t MIB_get_door_mode(const char *OID, value *v) {
    uint8_t door;
    uint8_t mode;

    if (equal(OID, MIB_DOORS_1_MODE)) {
        door = 1;
    } else if (equal(OID, MIB_DOORS_2_MODE)) {
        door = 2;
    } else if (equal(OID, MIB_DOORS_3_MODE)) {
        door = 3;
    } else if (equal(OID, MIB_DOORS_4_MODE)) {
        door = 4;
    }

    if (doors_get_mode(door, &mode)) {
        v->tag = VALUE_UINT8;
        v->integer = mode;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t MIB_get_door_delay(const char *OID, value *v) {
    uint8_t door;
    uint8_t delay;

    if (equal(OID, MIB_DOORS_1_DELAY)) {
        door = 1;
    } else if (equal(OID, MIB_DOORS_2_DELAY)) {
        door = 2;
    } else if (equal(OID, MIB_DOORS_3_DELAY)) {
        door = 3;
    } else if (equal(OID, MIB_DOORS_4_DELAY)) {
        door = 4;
    }

    if (doors_get_delay(door, &delay)) {
        v->tag = VALUE_UINT8;
        v->integer = delay;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t MIB_get_door_unlocked(const char *OID, value *v) {
    uint8_t door;
    bool unlocked;

    if (equal(OID, MIB_DOORS_1_UNLOCKED)) {
        door = 1;
    } else if (equal(OID, MIB_DOORS_2_UNLOCKED)) {
        door = 2;
    } else if (equal(OID, MIB_DOORS_3_UNLOCKED)) {
        door = 3;
    } else if (equal(OID, MIB_DOORS_4_UNLOCKED)) {
        door = 4;
    }

    if (doors_get_unlocked(door, &unlocked)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = !unlocked;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t MIB_get_door_open(const char *OID, value *v) {
    uint8_t door;
    bool open;

    if (equal(OID, MIB_DOORS_1_OPEN)) {
        door = 1;
    } else if (equal(OID, MIB_DOORS_2_OPEN)) {
        door = 2;
    } else if (equal(OID, MIB_DOORS_3_OPEN)) {
        door = 3;
    } else if (equal(OID, MIB_DOORS_4_OPEN)) {
        door = 4;
    }

    if (doors_get_open(door, &open)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = open;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t MIB_get_door_pushbutton(const char *OID, value *v) {
    uint8_t door;
    bool pressed;

    if (equal(OID, MIB_DOORS_1_BUTTON)) {
        door = 1;
    } else if (equal(OID, MIB_DOORS_2_BUTTON)) {
        door = 2;
    } else if (equal(OID, MIB_DOORS_3_BUTTON)) {
        door = 3;
    } else if (equal(OID, MIB_DOORS_4_BUTTON)) {
        door = 4;
    }

    if (doors_get_pushbutton(door, &pressed)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = pressed;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
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
