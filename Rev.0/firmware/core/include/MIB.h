#pragma once

#include <stdbool.h>

#include <encoding/ssmp/ssmp.h>

typedef struct MIBItem {
    uint32_t hash;
    const char *OID;
    int64_t (*get)(value *);
    int64_t (*set)(const value, value *);
} MIBItem;

extern const MIBItem MIB_SYS_BOARD_ID;
extern const MIBItem MIB_CONTROLLER_ID;
extern const MIBItem MIB_CONTROLLER_VERSION;
extern const MIBItem MIB_CONTROLLER_RELEASED;
extern const MIBItem MIB_CONTROLLER_DATETIME;
extern const MIBItem MIB_CONTROLLER_SYSERROR;
extern const MIBItem MIB_CONTROLLER_SYSINFO;
extern const MIBItem MIB_DOORS_1_MODE;
extern const MIBItem MIB_DOORS_2_MODE;
extern const MIBItem MIB_DOORS_3_MODE;
extern const MIBItem MIB_DOORS_4_MODE;
extern const MIBItem MIB_DOORS_1_DELAY;
extern const MIBItem MIB_DOORS_2_DELAY;
extern const MIBItem MIB_DOORS_3_DELAY;
extern const MIBItem MIB_DOORS_4_DELAY;
extern const MIBItem MIB_DOORS_1_UNLOCKED;
extern const MIBItem MIB_DOORS_2_UNLOCKED;
extern const MIBItem MIB_DOORS_3_UNLOCKED;
extern const MIBItem MIB_DOORS_4_UNLOCKED;
extern const MIBItem MIB_DOORS_1_OPEN;
extern const MIBItem MIB_DOORS_2_OPEN;
extern const MIBItem MIB_DOORS_3_OPEN;
extern const MIBItem MIB_DOORS_4_OPEN;
extern const MIBItem MIB_DOORS_1_BUTTON;
extern const MIBItem MIB_DOORS_2_BUTTON;
extern const MIBItem MIB_DOORS_3_BUTTON;
extern const MIBItem MIB_DOORS_4_BUTTON;
extern const MIBItem MIB_ALARMS_TAMPER_DETECT;
extern const MIBItem MIB_ALARMS_FIRE_ALARM;

extern const MIBItem OIDs[29];
