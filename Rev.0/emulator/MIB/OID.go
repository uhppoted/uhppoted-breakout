package MIB

type OID []uint32

var OIDs = map[string]OID{
	CONTROLLER_ID: []uint32{0, 1, 3, 6, 65536, 1, 1},
}
