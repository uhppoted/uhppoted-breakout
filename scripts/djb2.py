#!python3

OIDs = {
'MIB_BOARD_ID':            '0.1.3.6.1.4.1.65536.1.1',
'MIB_CONTROLLER_ID':       '0.1.3.6.1.4.1.65536.2.1',
'MIB_CONTROLLER_VERSION':  '0.1.3.6.1.4.1.65536.2.2',
'MIB_CONTROLLER_RELEASED': '0.1.3.6.1.4.1.65536.2.3',
'MIB_CONTROLLER_DATETIME': '0.1.3.6.1.4.1.65536.2.8',
'MIB_CONTROLLER_SYSERROR': '0.1.3.6.1.4.1.65536.2.9',
'MIB_CONTROLLER_SYSINFO':  '0.1.3.6.1.4.1.65536.2.10',

'MIB_DOORS_1_UNLOCKED': '0.1.3.6.1.4.1.65536.3.1.4',
'MIB_DOORS_2_UNLOCKED': '0.1.3.6.1.4.1.65536.3.2.4',
'MIB_DOORS_3_UNLOCKED': '0.1.3.6.1.4.1.65536.3.3.4',
'MIB_DOORS_4_UNLOCKED': '0.1.3.6.1.4.1.65536.3.4.4',

'MIB_DOORS_1_OPEN': '0.1.3.6.1.4.1.65536.3.1.5',
'MIB_DOORS_2_OPEN': '0.1.3.6.1.4.1.65536.3.2.5',
'MIB_DOORS_3_OPEN': '0.1.3.6.1.4.1.65536.3.3.5',
'MIB_DOORS_4_OPEN': '0.1.3.6.1.4.1.65536.3.4.5',

'MIB_DOORS_1_BUTTON': '0.1.3.6.1.4.1.65536.3.1.6',
'MIB_DOORS_2_BUTTON': '0.1.3.6.1.4.1.65536.3.2.6',
'MIB_DOORS_3_BUTTON': '0.1.3.6.1.4.1.65536.3.3.6',
'MIB_DOORS_4_BUTTON': '0.1.3.6.1.4.1.65536.3.4.6',
}

def djb2(s):
    hash = 5381
    for c in s:
        hash = ((hash << 5) + hash) + ord(c)
    return hash & 0x00ffffffff

with open('MIB.h', 'w') as f:
    f.write('#pragma once\n')
    f.write('\n')
    f.write("#include <encoding/ssmp/ssmp.h>\n")

    f.write('\n')
    for k,v in OIDs.items():
        f.write(f'#define {k:28} "{v}"\n')

    f.write('\n')
    for k,v in OIDs.items():
        f.write(f'#define HASH_{k:23} ({djb2(v)})\n')

    f.write('\n')
    f.write('value MIB_get(const char *OID);\n')
    f.write('int64_t MIB_set(const char *OID, const value u, value *v);\n')
    f.write('\n')

with open("struct.c", "w") as f:
    f.write('const struct {\n')
    f.write('    uint32_t hash;\n')
    f.write('    char *OID;\n')
    f.write('} MIB[] = {\n')

    for k,v in OIDs.items():
        f.write(f'    {{.hash = HASH_{k}, .OID = {k}}},\n')

    f.write('};\n');

