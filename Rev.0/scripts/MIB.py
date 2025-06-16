#!python3

from dataclasses import dataclass
from typing import Optional


@dataclass
class MIBItem:
    name: str
    OID: str
    get: Optional[str] = None
    set: Optional[str] = None


OIDs = [
    # sys
    MIBItem('MIB_SYS_BOARD_ID', '0.1.3.6.1.4.1.65536.1.1', 'MIB_get_sys_board_id'),

    # controller
    MIBItem('MIB_CONTROLLER_ID', '0.1.3.6.1.4.1.65536.2.1', 'MIB_get_controller_id'),
    MIBItem('MIB_CONTROLLER_VERSION', '0.1.3.6.1.4.1.65536.2.2', 'MIB_get_controller_version'),
    MIBItem('MIB_CONTROLLER_RELEASED', '0.1.3.6.1.4.1.65536.2.3', 'MIB_get_controller_released'),
    MIBItem('MIB_CONTROLLER_DATETIME', '0.1.3.6.1.4.1.65536.2.8', 'MIB_get_controller_datetime',
            'MIB_set_controller_datetime'),
    MIBItem('MIB_CONTROLLER_SYSERROR', '0.1.3.6.1.4.1.65536.2.9', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_RESTART', '0.1.3.6.1.4.1.65536.2.9.1', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_MEMORY', '0.1.3.6.1.4.1.65536.2.9.2', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_I2C', '0.1.3.6.1.4.1.65536.2.9.3', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_QUEUE', '0.1.3.6.1.4.1.65536.2.9.4', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_RX8900SA', '0.1.3.6.1.4.1.65536.2.9.5', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_U2', '0.1.3.6.1.4.1.65536.2.9.6', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_U3', '0.1.3.6.1.4.1.65536.2.9.7', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_U4', '0.1.3.6.1.4.1.65536.2.9.8', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_WATCHDOG', '0.1.3.6.1.4.1.65536.2.9.9', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_DEBUG', '0.1.3.6.1.4.1.65536.2.9.10', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSERROR_UNKNOWN', '0.1.3.6.1.4.1.65536.2.9.11', 'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSINFO', '0.1.3.6.1.4.1.65536.2.10', 'MIB_get_controller_sysinfo'),

    # door mode
    MIBItem('MIB_DOORS_1_MODE', '0.1.3.6.1.4.1.65536.3.1.1', 'MIB_get_door_mode', 'MIB_set_door_mode'),
    MIBItem('MIB_DOORS_2_MODE', '0.1.3.6.1.4.1.65536.3.2.1', 'MIB_get_door_mode', 'MIB_set_door_mode'),
    MIBItem('MIB_DOORS_3_MODE', '0.1.3.6.1.4.1.65536.3.3.1', 'MIB_get_door_mode', 'MIB_set_door_mode'),
    MIBItem('MIB_DOORS_4_MODE', '0.1.3.6.1.4.1.65536.3.4.1', 'MIB_get_door_mode', 'MIB_set_door_mode'),

    # door delay
    MIBItem('MIB_DOORS_1_DELAY', '0.1.3.6.1.4.1.65536.3.1.2', 'MIB_get_door_delay', 'MIB_set_door_delay'),
    MIBItem('MIB_DOORS_2_DELAY', '0.1.3.6.1.4.1.65536.3.2.2', 'MIB_get_door_delay', 'MIB_set_door_delay'),
    MIBItem('MIB_DOORS_3_DELAY', '0.1.3.6.1.4.1.65536.3.3.2', 'MIB_get_door_delay', 'MIB_set_door_delay'),
    MIBItem('MIB_DOORS_4_DELAY', '0.1.3.6.1.4.1.65536.3.4.2', 'MIB_get_door_delay', 'MIB_set_door_delay'),

    # door locks
    MIBItem('MIB_DOORS_1_UNLOCKED', '0.1.3.6.1.4.1.65536.3.1.4', 'MIB_get_door_unlocked'),
    MIBItem('MIB_DOORS_2_UNLOCKED', '0.1.3.6.1.4.1.65536.3.2.4', 'MIB_get_door_unlocked'),
    MIBItem('MIB_DOORS_3_UNLOCKED', '0.1.3.6.1.4.1.65536.3.3.4', 'MIB_get_door_unlocked'),
    MIBItem('MIB_DOORS_4_UNLOCKED', '0.1.3.6.1.4.1.65536.3.4.4', 'MIB_get_door_unlocked'),

    # door sensors
    MIBItem('MIB_DOORS_1_OPEN', '0.1.3.6.1.4.1.65536.3.1.5', 'MIB_get_door_open'),
    MIBItem('MIB_DOORS_2_OPEN', '0.1.3.6.1.4.1.65536.3.2.5', 'MIB_get_door_open'),
    MIBItem('MIB_DOORS_3_OPEN', '0.1.3.6.1.4.1.65536.3.3.5', 'MIB_get_door_open'),
    MIBItem('MIB_DOORS_4_OPEN', '0.1.3.6.1.4.1.65536.3.4.5', 'MIB_get_door_open'),

    # door pushbuttons
    MIBItem('MIB_DOORS_1_BUTTON', '0.1.3.6.1.4.1.65536.3.1.6', 'MIB_get_door_pushbutton'),
    MIBItem('MIB_DOORS_2_BUTTON', '0.1.3.6.1.4.1.65536.3.2.6', 'MIB_get_door_pushbutton'),
    MIBItem('MIB_DOORS_3_BUTTON', '0.1.3.6.1.4.1.65536.3.3.6', 'MIB_get_door_pushbutton'),
    MIBItem('MIB_DOORS_4_BUTTON', '0.1.3.6.1.4.1.65536.3.4.6', 'MIB_get_door_pushbutton'),

    # alarms
    MIBItem('MIB_ALARMS_TAMPER_DETECT', '0.1.3.6.1.4.1.65536.4.1', 'MIB_get_alarm_tamper_detect'),
    MIBItem('MIB_ALARMS_FIRE_ALARM', '0.1.3.6.1.4.1.65536.4.2', 'MIB_get_alarm_fire_alarm'),
]


def djb2(s):
    hash = 5381
    for c in s:
        hash = ((hash << 5) + hash) + ord(c)
    return hash & 0x00ffffffff


with open('../core/include/MIB.h', 'w') as f:
    f.write('#pragma once\n')
    f.write('\n')
    f.write('#include <stdbool.h>\n')
    f.write('\n')
    f.write('#include <encoding/ssmp/ssmp.h>\n')
    f.write('\n')
    f.write('typedef struct MIBItem {\n')
    f.write('    uint32_t hash;\n')
    f.write('    const char *OID;\n')
    f.write('    int64_t (*get)(const char *OID,value *);\n')
    f.write('    int64_t (*set)(const char *OID, const value, value *);\n')
    f.write('} MIBItem;\n')

    f.write('\n')
    for v in OIDs:
        f.write(f'extern const MIBItem {v.name};\n')

    f.write('\n')
    f.write(f'extern const MIBItem OIDs[{len(OIDs)}];\n')

    f.write('\n')

with open("../core/src/MIB/MIB.c", "w") as f:
    f.write('#include <string.h>\n')
    f.write('\n')
    f.write('#include <MIB.h>\n')
    f.write('#include <crypt/hash/djb2.h>\n')

    gets = list(dict.fromkeys([v.get for v in OIDs if v.get != None]))
    sets = list(dict.fromkeys([v.set for v in OIDs if v.set != None]))

    f.write('\n')
    for v in gets:
        f.write(f'extern int64_t {v}(const char *,value *);\n')

    f.write('\n')
    for v in sets:
        f.write(f'extern int64_t {v}(const char *,const value,value *);\n')

    f.write('\n')
    for v in OIDs:
        f.write(f'const MIBItem {v.name} = {{\n')
        f.write(f'    .hash = {djb2(v.OID)},\n')
        f.write(f'    .OID = "{v.OID}",\n')

        if v.get != None:
            f.write(f'    .get = {v.get},\n')

        if v.set != None:
            f.write(f'    .set = {v.set},\n')

        f.write('};\n')
        f.write('\n')

    f.write('\n')
    f.write(f'const MIBItem OIDs[{len(OIDs)}] = {{\n')
    for v in OIDs:
        f.write(f'    {v.name},\n')
    f.write('};\n')

    f.write('\n')
    f.write('bool equal(const char *OID, const MIBItem oid) {\n')
    f.write('    return (djb2(OID) == oid.hash) && (strcmp(OID, oid.OID) == 0);\n')
    f.write('}\n')

    f.write('\n')
