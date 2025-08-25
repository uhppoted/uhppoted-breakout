package UT0311

import (
	"ut0311/scmp"
)

var lookup = struct {
	modes     map[uint8]scmp.OID
	delays    map[uint8]scmp.OID
	passcodes map[uint8]scmp.OID
}{
	modes: map[uint8]scmp.OID{
		1: scmp.OID_DOORS_1_MODE,
		2: scmp.OID_DOORS_2_MODE,
		3: scmp.OID_DOORS_3_MODE,
		4: scmp.OID_DOORS_4_MODE,
	},
	delays: map[uint8]scmp.OID{
		1: scmp.OID_DOORS_1_DELAY,
		2: scmp.OID_DOORS_2_DELAY,
		3: scmp.OID_DOORS_3_DELAY,
		4: scmp.OID_DOORS_4_DELAY,
	},
	passcodes: map[uint8]scmp.OID{
		1: scmp.OID_DOORS_1_PASSCODES,
		2: scmp.OID_DOORS_2_PASSCODES,
		3: scmp.OID_DOORS_3_PASSCODES,
		4: scmp.OID_DOORS_4_PASSCODES,
	},
}
