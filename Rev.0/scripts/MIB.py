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
    MIBItem('MIB_CONTROLLER_DATETIME', '0.1.3.6.1.4.1.65536.2.8', 'MIB_get_controller_datetime', 'MIB_set_datetime'),
    MIBItem('MIB_CONTROLLER_SYSERROR', '0.1.3.6.1.4.1.65536.2.9', 'MIB_get_controller_syserror'),
    MIBItem('MIB_CONTROLLER_SYSINFO', '0.1.3.6.1.4.1.65536.2.10', 'MIB_get_controller_sysinfo'),

    # door locks
    MIBItem('MIB_DOORS_1_UNLOCKED', '0.1.3.6.1.4.1.65536.3.1.4', 'MIB_get_doors_1_unlocked'),
    MIBItem('MIB_DOORS_2_UNLOCKED', '0.1.3.6.1.4.1.65536.3.2.4', 'MIB_get_doors_2_unlocked'),
    MIBItem('MIB_DOORS_3_UNLOCKED', '0.1.3.6.1.4.1.65536.3.3.4', 'MIB_get_doors_3_unlocked'),
    MIBItem('MIB_DOORS_4_UNLOCKED', '0.1.3.6.1.4.1.65536.3.4.4', 'MIB_get_doors_4_unlocked'),

    # door sensors
    MIBItem('MIB_DOORS_1_OPEN', '0.1.3.6.1.4.1.65536.3.1.5', 'MIB_get_doors_1_open'),
    MIBItem('MIB_DOORS_2_OPEN', '0.1.3.6.1.4.1.65536.3.2.5', 'MIB_get_doors_2_open'),
    MIBItem('MIB_DOORS_3_OPEN', '0.1.3.6.1.4.1.65536.3.3.5', 'MIB_get_doors_3_open'),
    MIBItem('MIB_DOORS_4_OPEN', '0.1.3.6.1.4.1.65536.3.4.5', 'MIB_get_doors_4_open'),

    # door pushbuttons
    MIBItem('MIB_DOORS_1_BUTTON', '0.1.3.6.1.4.1.65536.3.1.6', 'MIB_get_doors_1_pushbutton'),
    MIBItem('MIB_DOORS_2_BUTTON', '0.1.3.6.1.4.1.65536.3.2.6', 'MIB_get_doors_2_pushbutton'),
    MIBItem('MIB_DOORS_3_BUTTON', '0.1.3.6.1.4.1.65536.3.3.6', 'MIB_get_doors_3_pushbutton'),
    MIBItem('MIB_DOORS_4_BUTTON', '0.1.3.6.1.4.1.65536.3.4.6', 'MIB_get_doors_4_pushbutton'),

    # alarms
    MIBItem('MIB_ALARMS_TAMPER_DETECT', '0.1.3.6.1.4.1.65536.4.1', 'MIB_get_alarms_tamper_detect'),
    MIBItem('MIB_ALARMS_FIRE_ALARM', '0.1.3.6.1.4.1.65536.4.2', 'MIB_get_alarms_fire_alarm'),
]

def djb2(s):
    hash = 5381
    for c in s:
        hash = ((hash << 5) + hash) + ord(c)
    return hash & 0x00ffffffff

with open('../core/include/MIB.h', 'w') as f:
    f.write('#pragma once\n')
    f.write('\n')
    f.write('#include <encoding/ssmp/ssmp.h>\n')
    f.write('\n')
    f.write('typedef struct MIBItem {\n')
    f.write('    uint32_t hash;\n')
    f.write('    const char *OID;\n')
    f.write('    value   (*get)();\n')
    f.write('    int64_t (*set)(const value, value *);\n')
    f.write('} MIBItem;\n')

    f.write('\n')
    for v in OIDs:
        f.write(f'extern const MIBItem {v.name};\n')

    f.write('\n')
    f.write(f'extern const MIBItem OIDs[{len(OIDs)}];\n')

    f.write('\n')
    f.write('value MIB_get(const char *OID);\n')
    f.write('int64_t MIB_set(const char *OID, const value u, value *v);\n')

with open("../core/src/MIB/MIB.c", "w") as f:
    f.write('#include <MIB.h>\n')

    f.write('\n')
    for v in OIDs:
        if v.get != None:
            f.write(f'extern value {v.get}();\n')

    f.write('\n')
    for v in OIDs:
        if v.set != None:
            f.write(f'extern int64_t {v.set}(const value,value *);\n')

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
