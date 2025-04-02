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

value MIB_get_boolean(const char *OID);

value MIB_get(const char *OID) {
    uint32_t hash = djb2(OID);

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

    if (strcmp(OID, MIB_CONTROLLER_SYSERROR) == 0) {
        v.tag = VALUE_UINT8;
        v.integer = (uint8_t)get_errors();
    }

    if (strcmp(OID, MIB_CONTROLLER_SYSINFO) == 0) {
        v.tag = VALUE_UINT8;
        v.integer = 0;
    }

    if (strcmp(OID, MIB_DOORS_1_UNLOCKED) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_2_UNLOCKED) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_3_UNLOCKED) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_4_UNLOCKED) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_1_OPEN) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_2_OPEN) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_3_OPEN) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_4_OPEN) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_1_BUTTON) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_2_BUTTON) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_3_BUTTON) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_DOORS_4_BUTTON) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_ALARMS_TAMPER_DETECT) == 0) {
        return MIB_get_boolean(OID);
    }

    if (strcmp(OID, MIB_ALARMS_FIRE_ALARM) == 0) {
        return MIB_get_boolean(OID);
    }

    return v;
}

value MIB_get_boolean(const char *OID) {
    value v = {
        .tag = VALUE_NULL,
    };

    if (strcmp(OID, MIB_DOORS_1_UNLOCKED) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U4_get_relay(1);
    }

    if (strcmp(OID, MIB_DOORS_2_UNLOCKED) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U4_get_relay(2);
    }

    if (strcmp(OID, MIB_DOORS_3_UNLOCKED) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U4_get_relay(3);
    }

    if (strcmp(OID, MIB_DOORS_4_UNLOCKED) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U4_get_relay(4);
    }

    if (strcmp(OID, MIB_DOORS_1_OPEN) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U3_get_door(1);
    }

    if (strcmp(OID, MIB_DOORS_2_OPEN) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U3_get_door(2);
    }

    if (strcmp(OID, MIB_DOORS_3_OPEN) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U3_get_door(3);
    }

    if (strcmp(OID, MIB_DOORS_4_OPEN) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U3_get_door(4);
    }

    if (strcmp(OID, MIB_DOORS_1_BUTTON) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U3_get_button(1);
    }

    if (strcmp(OID, MIB_DOORS_2_BUTTON) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U3_get_button(2);
    }

    if (strcmp(OID, MIB_DOORS_3_BUTTON) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U3_get_button(3);
    }

    if (strcmp(OID, MIB_DOORS_4_BUTTON) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = U3_get_button(4);
    }

    if (strcmp(OID, MIB_ALARMS_TAMPER_DETECT) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = false;
    }

    if (strcmp(OID, MIB_ALARMS_FIRE_ALARM) == 0) {
        v.tag = VALUE_BOOLEAN;
        v.boolean = false;
    }

    // FIXME return error ?

    return v;
}

