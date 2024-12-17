package impl

import (
	"fmt"
	"slices"

	"emulator/MIB"
	"emulator/log"
)

type Events struct {
}

func (e Events) GetUint8(oid MIB.OID) (uint8, error) {
	if index := MIB.Index(oid, MIB.OID_EVENTS_EVENT_GRANTED); index == 13579 {
		if slices.Equal(oid[9:], MIB.OID_EVENTS_EVENT_EVENT[9:]) {
			return 6, nil
		}

		if slices.Equal(oid[9:], MIB.OID_EVENTS_EVENT_DOOR[9:]) {
			return 4, nil
		}

		if slices.Equal(oid[9:], MIB.OID_EVENTS_EVENT_DIRECTION[9:]) {
			return 2, nil
		}

		if slices.Equal(oid[9:], MIB.OID_EVENTS_EVENT_REASON[9:]) {
			return 15, nil
		}
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetUint16(oid MIB.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetUint32(oid MIB.OID) (uint32, error) {
	if MIB.OID.Equal(oid, MIB.OID_EVENTS_CURRENT) {
		return 13579, nil
	}

	if index := MIB.Index(oid, MIB.OID_EVENTS_EVENT_CARD); index == 13579 {
		if slices.Equal(oid[9:], MIB.OID_EVENTS_EVENT_CARD[9:]) {
			return 10058400, nil
		}
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetBool(oid MIB.OID) (bool, error) {
	if index := MIB.Index(oid, MIB.OID_EVENTS_EVENT_EVENT); index == 13579 {
		if slices.Equal(oid[9:], MIB.OID_EVENTS_EVENT_GRANTED[9:]) {
			return true, nil
		}
	}

	return false, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetString(oid MIB.OID) (string, error) {
	if index := MIB.Index(oid, MIB.OID_EVENTS_EVENT_TIMESTAMP); index == 13579 {
		if slices.Equal(oid[9:], MIB.OID_EVENTS_EVENT_TIMESTAMP[9:]) {
			return "2024-12-17 15:52:45", nil
		}
	}

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
