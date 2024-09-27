package MIB

import ()

var system = struct {
	serialNo []uint8
}{
	serialNo: []uint8{0x12, 0x34, 0x56, 0x78},
}

func getSystemSerialNo() (any, error) {
	return system.serialNo, nil
}
