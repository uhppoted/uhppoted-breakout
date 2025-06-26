#!python3

from dataclasses import dataclass
from typing import Optional


@dataclass
class MIBItem:
    tag: str
    name: str
    OID: str
    get: Optional[str] = None
    set: Optional[str] = None


OIDs = [
    # sys
    MIBItem('system.board.id', 
            'MIB_SYS_BOARD_ID', 
            '0.1.3.6.1.4.1.65536.1.1', 
            'MIB_get_sys_board_id'),

    # controller
    MIBItem('controller.system.id', 
            'MIB_CONTROLLER_ID', 
            '0.1.3.6.1.4.1.65536.2.1', 
            'MIB_get_controller_id'),

    MIBItem('controller.system.version', 
            'MIB_CONTROLLER_VERSION', 
            '0.1.3.6.1.4.1.65536.2.2', 
            'MIB_get_controller_version'),
    
    MIBItem('controller.system.released', 
            'MIB_CONTROLLER_RELEASED', 
            '0.1.3.6.1.4.1.65536.2.3', 
            'MIB_get_controller_released'),

    MIBItem('controller.system.datetime', 
            'MIB_CONTROLLER_DATETIME', 
            '0.1.3.6.1.4.1.65536.2.8', 
            'MIB_get_controller_datetime',
            'MIB_set_controller_datetime'),

    MIBItem('controller.system.errors',
            'MIB_CONTROLLER_SYSERROR', 
            '0.1.3.6.1.4.1.65536.2.9', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.restart',
            'MIB_CONTROLLER_SYSERROR_RESTART', 
            '0.1.3.6.1.4.1.65536.2.9.1', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.memory',
            'MIB_CONTROLLER_SYSERROR_MEMORY', 
            '0.1.3.6.1.4.1.65536.2.9.2', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.I2C',
            'MIB_CONTROLLER_SYSERROR_I2C', 
            '0.1.3.6.1.4.1.65536.2.9.3', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.queue',
            'MIB_CONTROLLER_SYSERROR_QUEUE',
            '0.1.3.6.1.4.1.65536.2.9.4', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.RX8900SA',
            'MIB_CONTROLLER_SYSERROR_RX8900SA', 
            '0.1.3.6.1.4.1.65536.2.9.5', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    
    MIBItem('controller.system.errors.U2',
            'MIB_CONTROLLER_SYSERROR_U2', 
            '0.1.3.6.1.4.1.65536.2.9.6', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.U3',
            'MIB_CONTROLLER_SYSERROR_U3', 
            '0.1.3.6.1.4.1.65536.2.9.7', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.U4',
            'MIB_CONTROLLER_SYSERROR_U4', 
            '0.1.3.6.1.4.1.65536.2.9.8', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.watchdog',
            'MIB_CONTROLLER_SYSERROR_WATCHDOG', 
            '0.1.3.6.1.4.1.65536.2.9.9', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.debug',
            'MIB_CONTROLLER_SYSERROR_DEBUG',
            '0.1.3.6.1.4.1.65536.2.9.10', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),

    MIBItem('controller.system.errors.unknown',
            'MIB_CONTROLLER_SYSERROR_UNKNOWN', 
            '0.1.3.6.1.4.1.65536.2.9.11', 
            'MIB_get_controller_syserror',
            'MIB_set_controller_syserror'),
    
    MIBItem('controller.system.info',
            'MIB_CONTROLLER_SYSINFO', 
            '0.1.3.6.1.4.1.65536.2.10', 
            'MIB_get_controller_sysinfo'),

    # door mode
    MIBItem('controller.door.1.mode','MIB_DOORS_1_MODE', '0.1.3.6.1.4.1.65536.3.1.1', 'MIB_get_door_mode', 'MIB_set_door_mode'),
    MIBItem('controller.door.2.mode','MIB_DOORS_2_MODE', '0.1.3.6.1.4.1.65536.3.2.1', 'MIB_get_door_mode', 'MIB_set_door_mode'),
    MIBItem('controller.door.3.mode','MIB_DOORS_3_MODE', '0.1.3.6.1.4.1.65536.3.3.1', 'MIB_get_door_mode', 'MIB_set_door_mode'),
    MIBItem('controller.door.4.mode','MIB_DOORS_4_MODE', '0.1.3.6.1.4.1.65536.3.4.1', 'MIB_get_door_mode', 'MIB_set_door_mode'),

    # door delay
    MIBItem('controller.door.1.delay','MIB_DOORS_1_DELAY', '0.1.3.6.1.4.1.65536.3.1.2', 'MIB_get_door_delay', 'MIB_set_door_delay'),
    MIBItem('controller.door.2.delay','MIB_DOORS_2_DELAY', '0.1.3.6.1.4.1.65536.3.2.2', 'MIB_get_door_delay', 'MIB_set_door_delay'),
    MIBItem('controller.door.3.delay','MIB_DOORS_3_DELAY', '0.1.3.6.1.4.1.65536.3.3.2', 'MIB_get_door_delay', 'MIB_set_door_delay'),
    MIBItem('controller.door.4.delay','MIB_DOORS_4_DELAY', '0.1.3.6.1.4.1.65536.3.4.2', 'MIB_get_door_delay', 'MIB_set_door_delay'),

    # door locks
    MIBItem('controller.door.1.lock','MIB_DOORS_1_UNLOCKED', '0.1.3.6.1.4.1.65536.3.1.4', 'MIB_get_door_unlocked'),
    MIBItem('controller.door.2.lock','MIB_DOORS_2_UNLOCKED', '0.1.3.6.1.4.1.65536.3.2.4', 'MIB_get_door_unlocked'),
    MIBItem('controller.door.3.lock','MIB_DOORS_3_UNLOCKED', '0.1.3.6.1.4.1.65536.3.3.4', 'MIB_get_door_unlocked'),
    MIBItem('controller.door.4.lock','MIB_DOORS_4_UNLOCKED', '0.1.3.6.1.4.1.65536.3.4.4', 'MIB_get_door_unlocked'),

    # door sensors
    MIBItem('controller.door.1.open','MIB_DOORS_1_OPEN', '0.1.3.6.1.4.1.65536.3.1.5', 'MIB_get_door_open'),
    MIBItem('controller.door.2.open','MIB_DOORS_2_OPEN', '0.1.3.6.1.4.1.65536.3.2.5', 'MIB_get_door_open'),
    MIBItem('controller.door.3.open','MIB_DOORS_3_OPEN', '0.1.3.6.1.4.1.65536.3.3.5', 'MIB_get_door_open'),
    MIBItem('controller.door.4.open','MIB_DOORS_4_OPEN', '0.1.3.6.1.4.1.65536.3.4.5', 'MIB_get_door_open'),

    # door pushbuttons
    MIBItem('controller.door.1.button','MIB_DOORS_1_BUTTON', '0.1.3.6.1.4.1.65536.3.1.6', 'MIB_get_door_pushbutton'),
    MIBItem('controller.door.2.button','MIB_DOORS_2_BUTTON', '0.1.3.6.1.4.1.65536.3.2.6', 'MIB_get_door_pushbutton'),
    MIBItem('controller.door.3.button','MIB_DOORS_3_BUTTON', '0.1.3.6.1.4.1.65536.3.3.6', 'MIB_get_door_pushbutton'),
    MIBItem('controller.door.4.button','MIB_DOORS_4_BUTTON', '0.1.3.6.1.4.1.65536.3.4.6', 'MIB_get_door_pushbutton'),

    # alarms
    MIBItem('controller.alarm.tamper',
            'MIB_ALARMS_TAMPER_DETECT', 
            '0.1.3.6.1.4.1.65536.4.1', 
            'MIB_get_alarm_tamper_detect'),

    MIBItem('controller.alarm.fire',
            'MIB_ALARMS_FIRE_ALARM', 
            '0.1.3.6.1.4.1.65536.4.2', 
            'MIB_get_alarm_fire_alarm'),
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

# RPCD::MIB.lookup
with open("../../zero2w/rpcd/MIB/MIB.go", "w") as f:
    f.write('package MIB\n')
    f.write('\n')
    f.write('\n')

    f.write('var m = map[string]string{\n')
    for v in OIDs:
        f.write(f'    "{v.tag}": "{v.OID}",\n')
    f.write('}\n')
    f.write('\n')

    f.write('var r = map[string]string{\n')
    for v in OIDs:
        f.write(f'    "{v.OID}": "{v.tag}",\n')
    f.write('}\n')
    f.write('\n')

