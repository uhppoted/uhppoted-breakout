package MIB

type OID []uint32

var OIDs = map[string]OID{
	CONTROLLER_ID: []uint32{1, 3, 6, 1, 4, 1, 65536, 1, 1},
}
