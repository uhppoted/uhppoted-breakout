#include <string.h>

#include <MIB.h>
#include <crypt/hash/djb2.h>

extern int64_t MIB_get_sys_board_id(const char *, value *);
extern int64_t MIB_get_controller_id(const char *, value *);
extern int64_t MIB_get_controller_version(const char *, value *);
extern int64_t MIB_get_controller_released(const char *, value *);
extern int64_t MIB_get_controller_datetime(const char *, value *);
extern int64_t MIB_get_controller_syserror(const char *, value *);
extern int64_t MIB_get_controller_sysinfo(const char *, value *);
extern int64_t MIB_get_doors_interlock(const char *, value *);
extern int64_t MIB_get_door_mode(const char *, value *);
extern int64_t MIB_get_door_delay(const char *, value *);
extern int64_t MIB_get_door_unlocked(const char *, value *);
extern int64_t MIB_get_door_open(const char *, value *);
extern int64_t MIB_get_door_pushbutton(const char *, value *);
extern int64_t MIB_get_alarm_tamper_detect(const char *, value *);
extern int64_t MIB_get_alarm_fire_alarm(const char *, value *);

extern int64_t MIB_set_controller_datetime(const char *, const value, value *);
extern int64_t MIB_set_controller_syserror(const char *, const value, value *);
extern int64_t MIB_set_doors_interlock(const char *, const value, value *);
extern int64_t MIB_set_door_mode(const char *, const value, value *);
extern int64_t MIB_set_door_delay(const char *, const value, value *);
extern int64_t MIB_set_door_unlock(const char *, const value, value *);

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
    .set = MIB_set_controller_datetime,
};

const MIBItem MIB_CONTROLLER_SYSERROR = {
    .hash = 2957894743,
    .OID = "0.1.3.6.1.4.1.65536.2.9",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_RESTART = {
    .hash = 4216871990,
    .OID = "0.1.3.6.1.4.1.65536.2.9.1",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_MEMORY = {
    .hash = 4216871991,
    .OID = "0.1.3.6.1.4.1.65536.2.9.2",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_I2C = {
    .hash = 4216871992,
    .OID = "0.1.3.6.1.4.1.65536.2.9.3",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_QUEUE = {
    .hash = 4216871993,
    .OID = "0.1.3.6.1.4.1.65536.2.9.4",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_RX8900SA = {
    .hash = 4216871994,
    .OID = "0.1.3.6.1.4.1.65536.2.9.5",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_U2 = {
    .hash = 4216871995,
    .OID = "0.1.3.6.1.4.1.65536.2.9.6",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_U3 = {
    .hash = 4216871996,
    .OID = "0.1.3.6.1.4.1.65536.2.9.7",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_U4 = {
    .hash = 4216871997,
    .OID = "0.1.3.6.1.4.1.65536.2.9.8",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_WATCHDOG = {
    .hash = 4216871998,
    .OID = "0.1.3.6.1.4.1.65536.2.9.9",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_DEBUG = {
    .hash = 1717822246,
    .OID = "0.1.3.6.1.4.1.65536.2.9.10",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSERROR_UNKNOWN = {
    .hash = 1717822247,
    .OID = "0.1.3.6.1.4.1.65536.2.9.11",
    .get = MIB_get_controller_syserror,
    .set = MIB_set_controller_syserror,
};

const MIBItem MIB_CONTROLLER_SYSINFO = {
    .hash = 3121245791,
    .OID = "0.1.3.6.1.4.1.65536.2.10",
    .get = MIB_get_controller_sysinfo,
};

const MIBItem MIB_DOORS_INTERLOCK = {
    .hash = 4218048110,
    .OID = "0.1.3.6.1.4.1.65536.3.0.1",
    .get = MIB_get_doors_interlock,
    .set = MIB_set_doors_interlock,
};

const MIBItem MIB_DOOR_1_MODE = {
    .hash = 4218049199,
    .OID = "0.1.3.6.1.4.1.65536.3.1.1",
    .get = MIB_get_door_mode,
    .set = MIB_set_door_mode,
};

const MIBItem MIB_DOOR_2_MODE = {
    .hash = 4218050288,
    .OID = "0.1.3.6.1.4.1.65536.3.2.1",
    .get = MIB_get_door_mode,
    .set = MIB_set_door_mode,
};

const MIBItem MIB_DOOR_3_MODE = {
    .hash = 4218051377,
    .OID = "0.1.3.6.1.4.1.65536.3.3.1",
    .get = MIB_get_door_mode,
    .set = MIB_set_door_mode,
};

const MIBItem MIB_DOOR_4_MODE = {
    .hash = 4218052466,
    .OID = "0.1.3.6.1.4.1.65536.3.4.1",
    .get = MIB_get_door_mode,
    .set = MIB_set_door_mode,
};

const MIBItem MIB_DOOR_1_DELAY = {
    .hash = 4218049200,
    .OID = "0.1.3.6.1.4.1.65536.3.1.2",
    .get = MIB_get_door_delay,
    .set = MIB_set_door_delay,
};

const MIBItem MIB_DOOR_2_DELAY = {
    .hash = 4218050289,
    .OID = "0.1.3.6.1.4.1.65536.3.2.2",
    .get = MIB_get_door_delay,
    .set = MIB_set_door_delay,
};

const MIBItem MIB_DOOR_3_DELAY = {
    .hash = 4218051378,
    .OID = "0.1.3.6.1.4.1.65536.3.3.2",
    .get = MIB_get_door_delay,
    .set = MIB_set_door_delay,
};

const MIBItem MIB_DOOR_4_DELAY = {
    .hash = 4218052467,
    .OID = "0.1.3.6.1.4.1.65536.3.4.2",
    .get = MIB_get_door_delay,
    .set = MIB_set_door_delay,
};

const MIBItem MIB_DOOR_1_UNLOCKED = {
    .hash = 4218049202,
    .OID = "0.1.3.6.1.4.1.65536.3.1.4",
    .get = MIB_get_door_unlocked,
    .set = MIB_set_door_unlock,
};

const MIBItem MIB_DOOR_2_UNLOCKED = {
    .hash = 4218050291,
    .OID = "0.1.3.6.1.4.1.65536.3.2.4",
    .get = MIB_get_door_unlocked,
    .set = MIB_set_door_unlock,
};

const MIBItem MIB_DOOR_3_UNLOCKED = {
    .hash = 4218051380,
    .OID = "0.1.3.6.1.4.1.65536.3.3.4",
    .get = MIB_get_door_unlocked,
    .set = MIB_set_door_unlock,
};

const MIBItem MIB_DOOR_4_UNLOCKED = {
    .hash = 4218052469,
    .OID = "0.1.3.6.1.4.1.65536.3.4.4",
    .get = MIB_get_door_unlocked,
    .set = MIB_set_door_unlock,
};

const MIBItem MIB_DOOR_1_OPEN = {
    .hash = 4218049203,
    .OID = "0.1.3.6.1.4.1.65536.3.1.5",
    .get = MIB_get_door_open,
};

const MIBItem MIB_DOOR_2_OPEN = {
    .hash = 4218050292,
    .OID = "0.1.3.6.1.4.1.65536.3.2.5",
    .get = MIB_get_door_open,
};

const MIBItem MIB_DOOR_3_OPEN = {
    .hash = 4218051381,
    .OID = "0.1.3.6.1.4.1.65536.3.3.5",
    .get = MIB_get_door_open,
};

const MIBItem MIB_DOOR_4_OPEN = {
    .hash = 4218052470,
    .OID = "0.1.3.6.1.4.1.65536.3.4.5",
    .get = MIB_get_door_open,
};

const MIBItem MIB_DOOR_1_BUTTON = {
    .hash = 4218049204,
    .OID = "0.1.3.6.1.4.1.65536.3.1.6",
    .get = MIB_get_door_pushbutton,
};

const MIBItem MIB_DOOR_2_BUTTON = {
    .hash = 4218050293,
    .OID = "0.1.3.6.1.4.1.65536.3.2.6",
    .get = MIB_get_door_pushbutton,
};

const MIBItem MIB_DOOR_3_BUTTON = {
    .hash = 4218051382,
    .OID = "0.1.3.6.1.4.1.65536.3.3.6",
    .get = MIB_get_door_pushbutton,
};

const MIBItem MIB_DOOR_4_BUTTON = {
    .hash = 4218052471,
    .OID = "0.1.3.6.1.4.1.65536.3.4.6",
    .get = MIB_get_door_pushbutton,
};

const MIBItem MIB_DOOR_1_SWIPE = {
    .hash = 4218049205,
    .OID = "0.1.3.6.1.4.1.65536.3.1.7",
};

const MIBItem MIB_DOOR_2_SWIPE = {
    .hash = 4218050294,
    .OID = "0.1.3.6.1.4.1.65536.3.2.7",
};

const MIBItem MIB_DOOR_3_SWIPE = {
    .hash = 4218051383,
    .OID = "0.1.3.6.1.4.1.65536.3.3.7",
};

const MIBItem MIB_DOOR_4_SWIPE = {
    .hash = 4218052472,
    .OID = "0.1.3.6.1.4.1.65536.3.4.7",
};

const MIBItem MIB_DOOR_1_KEYCODE = {
    .hash = 4218049206,
    .OID = "0.1.3.6.1.4.1.65536.3.1.8",
};

const MIBItem MIB_DOOR_2_KEYCODE = {
    .hash = 4218050295,
    .OID = "0.1.3.6.1.4.1.65536.3.2.8",
};

const MIBItem MIB_DOOR_3_KEYCODE = {
    .hash = 4218051384,
    .OID = "0.1.3.6.1.4.1.65536.3.3.8",
};

const MIBItem MIB_DOOR_4_KEYCODE = {
    .hash = 4218052473,
    .OID = "0.1.3.6.1.4.1.65536.3.4.8",
};

const MIBItem MIB_ALARMS_TAMPER_DETECT = {
    .hash = 2957896913,
    .OID = "0.1.3.6.1.4.1.65536.4.1",
    .get = MIB_get_alarm_tamper_detect,
};

const MIBItem MIB_ALARMS_FIRE_ALARM = {
    .hash = 2957896914,
    .OID = "0.1.3.6.1.4.1.65536.4.2",
    .get = MIB_get_alarm_fire_alarm,
};

const MIBItem OIDs[49] = {
    MIB_SYS_BOARD_ID,
    MIB_CONTROLLER_ID,
    MIB_CONTROLLER_VERSION,
    MIB_CONTROLLER_RELEASED,
    MIB_CONTROLLER_DATETIME,
    MIB_CONTROLLER_SYSERROR,
    MIB_CONTROLLER_SYSERROR_RESTART,
    MIB_CONTROLLER_SYSERROR_MEMORY,
    MIB_CONTROLLER_SYSERROR_I2C,
    MIB_CONTROLLER_SYSERROR_QUEUE,
    MIB_CONTROLLER_SYSERROR_RX8900SA,
    MIB_CONTROLLER_SYSERROR_U2,
    MIB_CONTROLLER_SYSERROR_U3,
    MIB_CONTROLLER_SYSERROR_U4,
    MIB_CONTROLLER_SYSERROR_WATCHDOG,
    MIB_CONTROLLER_SYSERROR_DEBUG,
    MIB_CONTROLLER_SYSERROR_UNKNOWN,
    MIB_CONTROLLER_SYSINFO,
    MIB_DOORS_INTERLOCK,
    MIB_DOOR_1_MODE,
    MIB_DOOR_2_MODE,
    MIB_DOOR_3_MODE,
    MIB_DOOR_4_MODE,
    MIB_DOOR_1_DELAY,
    MIB_DOOR_2_DELAY,
    MIB_DOOR_3_DELAY,
    MIB_DOOR_4_DELAY,
    MIB_DOOR_1_UNLOCKED,
    MIB_DOOR_2_UNLOCKED,
    MIB_DOOR_3_UNLOCKED,
    MIB_DOOR_4_UNLOCKED,
    MIB_DOOR_1_OPEN,
    MIB_DOOR_2_OPEN,
    MIB_DOOR_3_OPEN,
    MIB_DOOR_4_OPEN,
    MIB_DOOR_1_BUTTON,
    MIB_DOOR_2_BUTTON,
    MIB_DOOR_3_BUTTON,
    MIB_DOOR_4_BUTTON,
    MIB_DOOR_1_SWIPE,
    MIB_DOOR_2_SWIPE,
    MIB_DOOR_3_SWIPE,
    MIB_DOOR_4_SWIPE,
    MIB_DOOR_1_KEYCODE,
    MIB_DOOR_2_KEYCODE,
    MIB_DOOR_3_KEYCODE,
    MIB_DOOR_4_KEYCODE,
    MIB_ALARMS_TAMPER_DETECT,
    MIB_ALARMS_FIRE_ALARM,
};

bool equal(const char *OID, const MIBItem oid) {
    return (djb2(OID) == oid.hash) && (strcmp(OID, oid.OID) == 0);
}
