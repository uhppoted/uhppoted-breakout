#pragma once

#include <stdbool.h>

#include <encoding/ssmp/ssmp.h>

typedef struct MIBItem {
    uint32_t hash;
    const char *OID;
    int64_t (*get)(const char *OID, value *);
    int64_t (*set)(const char *OID, const value, value *);
} MIBItem;

extern const MIBItem MIB_SYS_BOARD_ID;
extern const MIBItem MIB_CONTROLLER_ID;
extern const MIBItem MIB_CONTROLLER_VERSION;
extern const MIBItem MIB_CONTROLLER_RELEASED;
extern const MIBItem MIB_CONTROLLER_DATETIME;
extern const MIBItem MIB_CONTROLLER_SYSERROR;
extern const MIBItem MIB_CONTROLLER_SYSERROR_RESTART;
extern const MIBItem MIB_CONTROLLER_SYSERROR_MEMORY;
extern const MIBItem MIB_CONTROLLER_SYSERROR_I2C;
extern const MIBItem MIB_CONTROLLER_SYSERROR_QUEUE;
extern const MIBItem MIB_CONTROLLER_SYSERROR_RX8900SA;
extern const MIBItem MIB_CONTROLLER_SYSERROR_U2;
extern const MIBItem MIB_CONTROLLER_SYSERROR_U3;
extern const MIBItem MIB_CONTROLLER_SYSERROR_U4;
extern const MIBItem MIB_CONTROLLER_SYSERROR_WATCHDOG;
extern const MIBItem MIB_CONTROLLER_SYSERROR_DEBUG;
extern const MIBItem MIB_CONTROLLER_SYSERROR_UNKNOWN;
extern const MIBItem MIB_CONTROLLER_SYSINFO;
extern const MIBItem MIB_DOORS_INTERLOCK;
extern const MIBItem MIB_DOOR_1_MODE;
extern const MIBItem MIB_DOOR_2_MODE;
extern const MIBItem MIB_DOOR_3_MODE;
extern const MIBItem MIB_DOOR_4_MODE;
extern const MIBItem MIB_DOOR_1_DELAY;
extern const MIBItem MIB_DOOR_2_DELAY;
extern const MIBItem MIB_DOOR_3_DELAY;
extern const MIBItem MIB_DOOR_4_DELAY;
extern const MIBItem MIB_DOOR_1_UNLOCKED;
extern const MIBItem MIB_DOOR_2_UNLOCKED;
extern const MIBItem MIB_DOOR_3_UNLOCKED;
extern const MIBItem MIB_DOOR_4_UNLOCKED;
extern const MIBItem MIB_DOOR_1_OPEN;
extern const MIBItem MIB_DOOR_2_OPEN;
extern const MIBItem MIB_DOOR_3_OPEN;
extern const MIBItem MIB_DOOR_4_OPEN;
extern const MIBItem MIB_DOOR_1_BUTTON;
extern const MIBItem MIB_DOOR_2_BUTTON;
extern const MIBItem MIB_DOOR_3_BUTTON;
extern const MIBItem MIB_DOOR_4_BUTTON;
extern const MIBItem MIB_DOOR_1_SWIPE;
extern const MIBItem MIB_DOOR_2_SWIPE;
extern const MIBItem MIB_DOOR_3_SWIPE;
extern const MIBItem MIB_DOOR_4_SWIPE;
extern const MIBItem MIB_DOOR_1_KEYCODE;
extern const MIBItem MIB_DOOR_2_KEYCODE;
extern const MIBItem MIB_DOOR_3_KEYCODE;
extern const MIBItem MIB_DOOR_4_KEYCODE;
extern const MIBItem MIB_DOOR_1_ACCESS;
extern const MIBItem MIB_DOOR_2_ACCESS;
extern const MIBItem MIB_DOOR_3_ACCESS;
extern const MIBItem MIB_DOOR_4_ACCESS;
extern const MIBItem MIB_ALARMS_TAMPER_DETECT;
extern const MIBItem MIB_ALARMS_FIRE_ALARM;

extern const MIBItem OIDs[53];
