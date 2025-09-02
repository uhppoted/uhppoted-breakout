#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/time.h>

#include <crypt/hash/djb2.h>

#include <MIB.h>
#include <RTC.h>
#include <SSMP.h>
#include <breakout.h>
#include <doors.h>
#include <log.h>

#define LOGTAG "MIB"

extern bool equal(const char *OID, const MIBItem oid);
extern int64_t MIB_get_controller_syserror(const char *, value *);

int64_t MIB_set(const char *OID, const value u, value *v) {
    uint32_t hash = djb2(OID);

    int N = sizeof(OIDs) / sizeof(MIBItem);
    for (int i = 0; i < N; i++) {
        MIBItem item = OIDs[i];

        if (hash == item.hash && strcmp(OID, item.OID) == 0) {
            if (item.set == NULL) {
                return SSMP_ERROR_READONLY;
            }

            return item.set(OID, u, v);
        }
    }

    return SSMP_ERROR_NO_SUCH_OBJECT;
}

int64_t MIB_set_controller_datetime(const char *OID, const value u, value *v) {
    // ... check value type
    if (u.tag != VALUE_OCTET_STRING) {
        return SSMP_ERROR_WRONG_TYPE;
    }

    // ... RTC initialised?
    if (!RTC_ready()) {
        return SSMP_ERROR_NOT_WRITABLE;
    }

    // ... set RTC time
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int rc;

    const char *s = u.octets.bytes;

    if ((rc = sscanf(s, "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second)) != 6) {
        return SSMP_ERROR_BAD_VALUE;
    } else if (!RTC_set_datetime(year, month, day, hour, minute, second)) {
        warnf(LOGTAG, "error setting RTC datetime %04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
        return SSMP_ERROR_COMMIT_FAILED;
    }

    // ... delay to let RTC time (hopefully) propagate
    sleep_ms(5);

    // ... get current RTC time
    uint16_t yyyy;
    uint8_t mm;
    uint8_t dd;
    uint8_t HH;
    uint8_t MM;
    uint8_t SS;
    int N;

    if (!RTC_get_datetime(&yyyy, &mm, &dd, &HH, &MM, &SS, NULL)) {
        return SSMP_ERROR_COMMIT_FAILED;
    }

    // ... check delta < 1s
    uint64_t t1 = datetime_to_epoch(year, month, day, hour, minute, second);
    uint64_t t2 = datetime_to_epoch(yyyy, mm, dd, HH, MM, SS);
    int64_t dt = t1 < t2 ? (int64_t)(t2 - t1) : (int64_t)(t1 - t2);

    if (dt > 1) {
        return SSMP_ERROR_COMMIT_FAILED;
    }

    // ... return current RTC time
    slice octets = {
        .capacity = 32,
        .length = 0,
        .bytes = (char *)calloc(32, sizeof(uint8_t)),
    };

    N = snprintf(octets.bytes, octets.capacity, "%04u-%02u-%02u %02u:%02u:%02u", yyyy, mm, dd, HH, MM, SS);
    octets.length = N < 0 ? 0 : (N < 19 ? N : 19);

    v->tag = VALUE_OCTET_STRING;
    v->octets = octets;

    return SSMP_ERROR_NONE;
}

int64_t MIB_set_controller_syserror(const char *OID, const value u, value *v) {
    if (u.tag != VALUE_BOOLEAN) {
        return SSMP_ERROR_WRONG_TYPE;
    }

    if (u.boolean != false) {
        return SSMP_ERROR_BAD_VALUE;
    }

    if (equal(OID, MIB_CONTROLLER_SYSERROR)) {
        syserr_clear(ERR_MEMORY);
        syserr_clear(ERR_I2C);
        syserr_clear(ERR_QUEUE);
        syserr_clear(ERR_RX8900SA);
        syserr_clear(ERR_U2);
        syserr_clear(ERR_U3);
        syserr_clear(ERR_U4);
        syserr_clear(ERR_WATCHDOG);
        syserr_clear(ERR_DEBUG);
        syserr_clear(ERR_UNKNOWN);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_MEMORY)) {
        syserr_clear(ERR_MEMORY);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_I2C)) {
        syserr_clear(ERR_I2C);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_QUEUE)) {
        syserr_clear(ERR_QUEUE);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_RX8900SA)) {
        syserr_clear(ERR_RX8900SA);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_U2)) {
        syserr_clear(ERR_U2);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_U3)) {
        syserr_clear(ERR_U3);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_U4)) {
        syserr_clear(ERR_U4);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_WATCHDOG)) {
        syserr_clear(ERR_WATCHDOG);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_DEBUG)) {
        syserr_clear(ERR_DEBUG);
    } else if (equal(OID, MIB_CONTROLLER_SYSERROR_DEBUG)) {
        syserr_clear(ERR_UNKNOWN);
    }

    return MIB_get_controller_syserror(OID, v);
}

int64_t MIB_set_doors_interlock(const char *OID, const value u, value *v) {
    if (u.tag != VALUE_UINT8 && u.tag != VALUE_UINT32) {
        return SSMP_ERROR_WRONG_TYPE;
    }

    if (u.integer != 0 && u.integer != 1 && u.integer != 2 && u.integer != 3 && u.integer != 4 && u.integer != 8) {
        return SSMP_ERROR_BAD_VALUE;
    }

    uint8_t interlock = (uint8_t)u.integer;

    if (!doors_set_interlock(interlock)) {
        return SSMP_ERROR_COMMIT_FAILED;
    }

    if (doors_get_interlock(&interlock)) {
        v->tag = VALUE_UINT8;
        v->integer = interlock;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_INTERNAL;
}

int64_t MIB_set_door_mode(const char *OID, const value u, value *v) {
    if (u.tag != VALUE_UINT8 && u.tag != VALUE_UINT16 && u.tag != VALUE_UINT32) {
        return SSMP_ERROR_WRONG_TYPE;
    }

    if (u.integer != 1 && u.integer != 2 && u.integer != 3) {
        return SSMP_ERROR_BAD_VALUE;
    }

    uint8_t mode = (uint8_t)u.integer;
    uint8_t door = 0;

    if (equal(OID, MIB_DOOR_1_MODE)) {
        door = 1;
    } else if (equal(OID, MIB_DOOR_2_MODE)) {
        door = 2;
    } else if (equal(OID, MIB_DOOR_3_MODE)) {
        door = 3;
    } else if (equal(OID, MIB_DOOR_4_MODE)) {
        door = 4;
    }

    if (!doors_set_mode(door, mode)) {
        return SSMP_ERROR_COMMIT_FAILED;
    }

    if (doors_get_mode(door, &mode)) {
        v->tag = VALUE_UINT8;
        v->integer = mode;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t MIB_set_door_delay(const char *OID, const value u, value *v) {
    if (u.tag != VALUE_UINT8 && u.tag != VALUE_UINT16 && u.tag != VALUE_UINT32) {
        return SSMP_ERROR_WRONG_TYPE;
    }

    uint8_t delay = (uint8_t)u.integer;
    uint8_t door;

    if (equal(OID, MIB_DOOR_1_DELAY)) {
        door = 1;
    } else if (equal(OID, MIB_DOOR_2_DELAY)) {
        door = 2;
    } else if (equal(OID, MIB_DOOR_3_DELAY)) {
        door = 3;
    } else if (equal(OID, MIB_DOOR_4_DELAY)) {
        door = 4;
    }

    if (!doors_set_delay(door, delay)) {
        return SSMP_ERROR_COMMIT_FAILED;
    }

    if (doors_get_delay(door, &delay)) {
        v->tag = VALUE_UINT8;
        v->integer = delay;
        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}

int64_t MIB_set_door_unlock(const char *OID, const value u, value *v) {
    if (u.tag != VALUE_BOOLEAN) {
        return SSMP_ERROR_WRONG_TYPE;
    }

    bool unlock = (bool)u.boolean;
    uint8_t door;

    if (equal(OID, MIB_DOOR_1_UNLOCKED)) {
        door = 1;
    } else if (equal(OID, MIB_DOOR_2_UNLOCKED)) {
        door = 2;
    } else if (equal(OID, MIB_DOOR_3_UNLOCKED)) {
        door = 3;
    } else if (equal(OID, MIB_DOOR_4_UNLOCKED)) {
        door = 4;
    } else {
        return SSMP_ERROR_NO_SUCH_OBJECT;
    }

    // ... normally closed?
    uint8_t mode;
    if (!doors_get_mode(door, &mode)) {
        return SSMP_ERROR_INTERNAL;
    } else if (unlock && mode != NORMALLY_OPEN && mode != CONTROLLED) {
        return SSMP_ERROR_BAD_VALUE;
    }

    // ... unlock
    if (unlock && !doors_unlock(door)) {
        return SSMP_ERROR_COMMIT_FAILED;
    } else if (!unlock && !doors_lock(door)) {
        return SSMP_ERROR_COMMIT_FAILED;
    }

    // ... read back
    bool unlocked;
    if (doors_get_unlocked(door, &unlocked)) {
        v->tag = VALUE_BOOLEAN;
        v->boolean = !unlocked;

        return SSMP_ERROR_NONE;
    }

    return SSMP_ERROR_NO_ACCESS;
}
