package events

import (
	"fmt"

	"emulator/log"
	"emulator/scmp"
)

type Events struct {
}

func (e Events) GetUint8(oid scmp.OID) (uint8, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_EVENT) {
		if index := scmp.Index(oid); index == 13579 {
			return 6, nil
		}
	}
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_DOOR) {
		if index := scmp.Index(oid); index == 13579 {
			return 4, nil
		}
	}
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_DIRECTION) {
		if index := scmp.Index(oid); index == 13579 {
			return 2, nil
		}
	}
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_REASON) {
		if index := scmp.Index(oid); index == 13579 {
			return 15, nil
		}
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetUint16(oid scmp.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetUint32(oid scmp.OID) (uint32, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_CURRENT) {
		return 13579, nil
	}

	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_CARD) {
		if index := scmp.Index(oid); index == 13579 {
			return 10058400, nil
		}
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetBool(oid scmp.OID) (bool, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_GRANTED) {
		if index := scmp.Index(oid); index == 13579 {
			return true, nil
		}
	}

	return false, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetString(oid scmp.OID) (string, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_TIMESTAMP) {
		if index := scmp.Index(oid); index == 13579 {
			return "2024-12-17 15:52:45", nil
		}
	}

	return "", fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetOctets(oid scmp.OID) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) SetUint8(oid scmp.OID, val uint8) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) SetString(oid scmp.OID, value string) (string, error) {
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
