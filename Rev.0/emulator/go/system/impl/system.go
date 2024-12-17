package impl

import (
	"fmt"

	"emulator/MIB"
	"emulator/log"
)

type System struct {
}

func (sys System) GetUint8(oid MIB.OID) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetUint16(oid MIB.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetUint32(oid MIB.OID) (uint32, error) {
	if MIB.OID.Equal(oid, MIB.OID_CONTROLLER_SEQUENCE_NUMBER) {
		return 0, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetBool(oid MIB.OID) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetString(oid MIB.OID) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func debugf(format string, args ...any) {
	log.Debugf("SYS", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("SYS", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("SYS", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("SYS", format, args...)
}
