#pragma once

#include <encoding/ssmp/ssmp.h>

typedef struct MIBItem {
    uint32_t hash;
    const char *OID;
    value (*get)();
} MIBItem;

extern const MIBItem MIB_BOARD_ID;
extern const MIBItem MIB_CONTROLLER_ID;
extern const MIBItem MIB_CONTROLLER_VERSION;
extern const MIBItem MIB_CONTROLLER_RELEASED;
extern const MIBItem MIB_CONTROLLER_DATETIME;
extern const MIBItem MIB_CONTROLLER_SYSERROR;
extern const MIBItem MIB_CONTROLLER_SYSINFO;
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

value MIB_get(const char *OID);
int64_t MIB_set(const char *OID, const value u, value *v);
