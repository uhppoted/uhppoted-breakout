package cards

import (
	"fmt"

	"emulator/log"
	"emulator/scmp"
)

type Cards struct {
}

func (c Cards) GetUint8(oid scmp.OID) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedUint8(oid scmp.OID, index uint32) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetUint16(oid scmp.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedUint16(oid scmp.OID, index uint32) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetUint32(oid scmp.OID) (uint32, error) {
	if scmp.Is(oid, scmp.OID_CARDS_COUNT) {
		return 29, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedUint32(oid scmp.OID, index uint32) (uint32, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetBool(oid scmp.OID) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedBool(oid scmp.OID, index uint32) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetString(oid scmp.OID) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedString(oid scmp.OID, index uint32) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetOctets(oid scmp.OID) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedOctets(oid scmp.OID, index uint32) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetUint8(oid scmp.OID, val uint8) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetString(oid scmp.OID, value string) (string, error) {
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
