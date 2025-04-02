#!python3

OIDs = {
     # sys
    'MIB_BOARD_ID': '0.1.3.6.1.4.1.65536.1.1',

     # controller
    'MIB_CONTROLLER_ID': '0.1.3.6.1.4.1.65536.2.1',
    'MIB_CONTROLLER_VERSION': '0.1.3.6.1.4.1.65536.2.2',
    'MIB_CONTROLLER_RELEASED': '0.1.3.6.1.4.1.65536.2.3',
    'MIB_CONTROLLER_DATETIME': '0.1.3.6.1.4.1.65536.2.8',
    'MIB_CONTROLLER_SYSERROR': '0.1.3.6.1.4.1.65536.2.9',
    'MIB_CONTROLLER_SYSINFO': '0.1.3.6.1.4.1.65536.2.10',

     # door locks
    'MIB_DOORS_1_UNLOCKED': '0.1.3.6.1.4.1.65536.3.1.4',
    'MIB_DOORS_2_UNLOCKED': '0.1.3.6.1.4.1.65536.3.2.4',
    'MIB_DOORS_3_UNLOCKED': '0.1.3.6.1.4.1.65536.3.3.4',
    'MIB_DOORS_4_UNLOCKED': '0.1.3.6.1.4.1.65536.3.4.4',

     # door sensors
    'MIB_DOORS_1_OPEN': '0.1.3.6.1.4.1.65536.3.1.5',
    'MIB_DOORS_2_OPEN': '0.1.3.6.1.4.1.65536.3.2.5',
    'MIB_DOORS_3_OPEN': '0.1.3.6.1.4.1.65536.3.3.5',
    'MIB_DOORS_4_OPEN': '0.1.3.6.1.4.1.65536.3.4.5',

     # pushbuttons
    'MIB_DOORS_1_BUTTON': '0.1.3.6.1.4.1.65536.3.1.6',
    'MIB_DOORS_2_BUTTON': '0.1.3.6.1.4.1.65536.3.2.6',
    'MIB_DOORS_3_BUTTON': '0.1.3.6.1.4.1.65536.3.3.6',
    'MIB_DOORS_4_BUTTON': '0.1.3.6.1.4.1.65536.3.4.6',

     # alarms
    'MIB_ALARMS_TAMPER_DETECT': '0.1.3.6.1.4.1.65536.4.1',
    'MIB_ALARMS_FIRE_ALARM': '0.1.3.6.1.4.1.65536.4.2',
}


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
    f.write('    const char *OID;\n');
    f.write('} MIBItem;\n')

    f.write('\n')
    for k, v in OIDs.items():
        f.write(f'extern const MIBItem {k};\n')

    f.write('\n')
    f.write('value MIB_get(const char *OID);\n')
    f.write('int64_t MIB_set(const char *OID, const value u, value *v);\n')
    f.write('\n')

with open("../core/src/MIB/MIB.c", "w") as f:
    f.write('#include <MIB.h>\n')
    f.write('\n')

    for k, v in OIDs.items():
        f.write(f'const MIBItem {k} = {{\n')
        f.write(f'    .hash = {djb2(v)},\n')
        f.write(f'    .OID = "{v}",\n')
        f.write('};\n')
        f.write('\n')

    f.write('\n')
    f.write(f'const MIBItem OIDs[{len(OIDs)}] = {{\n')
    for k, v in OIDs.items():
        f.write(f'    {k},\n')
    f.write('};\n')
