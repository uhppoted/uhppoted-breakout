package system

import (
	"fmt"

	"emulator/log"
	"emulator/scmp"
)

type System struct {
}

func (sys System) GetUint8(oid scmp.OID) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetUint16(oid scmp.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetUint32(oid scmp.OID) (uint32, error) {
	if scmp.Is(oid, scmp.OID_CONTROLLER_SEQUENCE_NUMBER) {
		return 0, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetBool(oid scmp.OID) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetString(oid scmp.OID) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetOctets(oid scmp.OID) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (sys *System) SetUint8(oid scmp.OID, val uint8) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys *System) SetString(oid scmp.OID, value string) (string, error) {
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
