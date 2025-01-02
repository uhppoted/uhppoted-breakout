package UT0311

import (
	"emulator/scmp"
)

var modes = map[uint8]scmp.OID{
	1: scmp.OID_DOORS_1_MODE,
	2: scmp.OID_DOORS_2_MODE,
	3: scmp.OID_DOORS_3_MODE,
	4: scmp.OID_DOORS_4_MODE,
}

var delays = map[uint8]scmp.OID{
	1: scmp.OID_DOORS_1_DELAY,
	2: scmp.OID_DOORS_2_DELAY,
	3: scmp.OID_DOORS_3_DELAY,
	4: scmp.OID_DOORS_4_DELAY,
}
