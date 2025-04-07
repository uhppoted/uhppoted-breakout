#include <MIB.h>

extern int64_t MIB_get_sys_board_id(value *);
extern int64_t MIB_get_controller_id(value *);
extern int64_t MIB_get_controller_version(value *);
extern int64_t MIB_get_controller_released(value *);
extern int64_t MIB_get_controller_datetime(value *);
extern int64_t MIB_get_controller_syserror(value *);
extern int64_t MIB_get_controller_sysinfo(value *);
extern int64_t MIB_get_doors_1_unlocked(value *);
extern int64_t MIB_get_doors_2_unlocked(value *);
extern int64_t MIB_get_doors_3_unlocked(value *);
extern int64_t MIB_get_doors_4_unlocked(value *);
extern int64_t MIB_get_doors_1_open(value *);
extern int64_t MIB_get_doors_2_open(value *);
extern int64_t MIB_get_doors_3_open(value *);
extern int64_t MIB_get_doors_4_open(value *);
extern int64_t MIB_get_doors_1_pushbutton(value *);
extern int64_t MIB_get_doors_2_pushbutton(value *);
extern int64_t MIB_get_doors_3_pushbutton(value *);
extern int64_t MIB_get_doors_4_pushbutton(value *);
extern int64_t MIB_get_alarms_tamper_detect(value *);
extern int64_t MIB_get_alarms_fire_alarm(value *);

extern int64_t MIB_set_datetime(const value, value *);

const MIBItem MIB_SYS_BOARD_ID = {
    .hash = 2957893646,
    .OID = "0.1.3.6.1.4.1.65536.1.1",
    .get = MIB_get_sys_board_id,
};

const MIBItem MIB_CONTROLLER_ID = {
    .hash = 2957894735,
    .OID = "0.1.3.6.1.4.1.65536.2.1",
    .get = MIB_get_controller_id,
};

const MIBItem MIB_CONTROLLER_VERSION = {
    .hash = 2957894736,
    .OID = "0.1.3.6.1.4.1.65536.2.2",
    .get = MIB_get_controller_version,
};

const MIBItem MIB_CONTROLLER_RELEASED = {
    .hash = 2957894737,
    .OID = "0.1.3.6.1.4.1.65536.2.3",
    .get = MIB_get_controller_released,
};

const MIBItem MIB_CONTROLLER_DATETIME = {
    .hash = 2957894742,
    .OID = "0.1.3.6.1.4.1.65536.2.8",
    .get = MIB_get_controller_datetime,
    .set = MIB_set_datetime,
};

const MIBItem MIB_CONTROLLER_SYSERROR = {
    .hash = 2957894743,
    .OID = "0.1.3.6.1.4.1.65536.2.9",
    .get = MIB_get_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSINFO = {
    .hash = 3121245791,
    .OID = "0.1.3.6.1.4.1.65536.2.10",
    .get = MIB_get_controller_sysinfo,
};

const MIBItem MIB_DOORS_1_UNLOCKED = {
    .hash = 4218049202,
    .OID = "0.1.3.6.1.4.1.65536.3.1.4",
    .get = MIB_get_doors_1_unlocked,
};

const MIBItem MIB_DOORS_2_UNLOCKED = {
    .hash = 4218050291,
    .OID = "0.1.3.6.1.4.1.65536.3.2.4",
    .get = MIB_get_doors_2_unlocked,
};

const MIBItem MIB_DOORS_3_UNLOCKED = {
    .hash = 4218051380,
    .OID = "0.1.3.6.1.4.1.65536.3.3.4",
    .get = MIB_get_doors_3_unlocked,
};

const MIBItem MIB_DOORS_4_UNLOCKED = {
    .hash = 4218052469,
    .OID = "0.1.3.6.1.4.1.65536.3.4.4",
    .get = MIB_get_doors_4_unlocked,
};

const MIBItem MIB_DOORS_1_OPEN = {
    .hash = 4218049203,
    .OID = "0.1.3.6.1.4.1.65536.3.1.5",
    .get = MIB_get_doors_1_open,
};

const MIBItem MIB_DOORS_2_OPEN = {
    .hash = 4218050292,
    .OID = "0.1.3.6.1.4.1.65536.3.2.5",
    .get = MIB_get_doors_2_open,
};

const MIBItem MIB_DOORS_3_OPEN = {
    .hash = 4218051381,
    .OID = "0.1.3.6.1.4.1.65536.3.3.5",
    .get = MIB_get_doors_3_open,
};

const MIBItem MIB_DOORS_4_OPEN = {
    .hash = 4218052470,
    .OID = "0.1.3.6.1.4.1.65536.3.4.5",
    .get = MIB_get_doors_4_open,
};

const MIBItem MIB_DOORS_1_BUTTON = {
    .hash = 4218049204,
    .OID = "0.1.3.6.1.4.1.65536.3.1.6",
    .get = MIB_get_doors_1_pushbutton,
};

const MIBItem MIB_DOORS_2_BUTTON = {
    .hash = 4218050293,
    .OID = "0.1.3.6.1.4.1.65536.3.2.6",
    .get = MIB_get_doors_2_pushbutton,
};

const MIBItem MIB_DOORS_3_BUTTON = {
    .hash = 4218051382,
    .OID = "0.1.3.6.1.4.1.65536.3.3.6",
    .get = MIB_get_doors_3_pushbutton,
};

const MIBItem MIB_DOORS_4_BUTTON = {
    .hash = 4218052471,
    .OID = "0.1.3.6.1.4.1.65536.3.4.6",
    .get = MIB_get_doors_4_pushbutton,
};

const MIBItem MIB_ALARMS_TAMPER_DETECT = {
    .hash = 2957896913,
    .OID = "0.1.3.6.1.4.1.65536.4.1",
    .get = MIB_get_alarms_tamper_detect,
};

const MIBItem MIB_ALARMS_FIRE_ALARM = {
    .hash = 2957896914,
    .OID = "0.1.3.6.1.4.1.65536.4.2",
    .get = MIB_get_alarms_fire_alarm,
};

const MIBItem OIDs[21] = {
    MIB_SYS_BOARD_ID,
    MIB_CONTROLLER_ID,
    MIB_CONTROLLER_VERSION,
    MIB_CONTROLLER_RELEASED,
    MIB_CONTROLLER_DATETIME,
    MIB_CONTROLLER_SYSERROR,
    MIB_CONTROLLER_SYSINFO,
    MIB_DOORS_1_UNLOCKED,
    MIB_DOORS_2_UNLOCKED,
    MIB_DOORS_3_UNLOCKED,
    MIB_DOORS_4_UNLOCKED,
    MIB_DOORS_1_OPEN,
    MIB_DOORS_2_OPEN,
    MIB_DOORS_3_OPEN,
    MIB_DOORS_4_OPEN,
    MIB_DOORS_1_BUTTON,
    MIB_DOORS_2_BUTTON,
    MIB_DOORS_3_BUTTON,
    MIB_DOORS_4_BUTTON,
    MIB_ALARMS_TAMPER_DETECT,
    MIB_ALARMS_FIRE_ALARM,
};
