package events

import (
	"fmt"
	"sync/atomic"
	"time"

	"emulator/driver/rpcd"
	"emulator/log"
	"emulator/scmp"
)

type Event struct {
	Index     uint32    `json:"index"`
	Type      EventType `json:"type"`
	Granted   bool      `json:"granted"`
	Door      uint8     `json:"door"`
	Direction uint8     `json:"direction"`
	Card      uint32    `json:"card"`
	Timestamp time.Time `json:"timestamp,omitempty"`
	Reason    Reason    `json:"reason"`
}

type Events struct {
	recordAll bool
	index     uint32
}

func NewEvents() *Events {
	return &Events{
		recordAll: false,
	}
}

var Index atomic.Uint32

// type Event struct {
// 	Timestamp time.Time
// 	ID        uint32
// 	Category  uint32
// 	Event     uint32
// 	OID       string
// 	Value     any
// }

func (e *Events) Add(event rpcd.Event) Event {
	evt := Event{
		Index:     Index.Add(1),
		Type:      lookup(event.OID),
		Granted:   false,
		Door:      door(event.OID),
		Direction: 0,
		Card:      0,
		Timestamp: event.Timestamp,
		Reason:    reason(event.OID, event.Value),
	}

	return evt
}

func (e *Events) GetUint8(oid scmp.OID) (uint8, error) {
	if index, ok := scmp.Index(oid, scmp.OID_EVENTS_EVENT_EVENT); ok && index == 13579 {
		return 6, nil
	}

	if index, ok := scmp.Index(oid, scmp.OID_EVENTS_EVENT_DOOR); ok && index == 13579 {
		return 4, nil
	}

	if index, ok := scmp.Index(oid, scmp.OID_EVENTS_EVENT_DIRECTION); ok && index == 13579 {
		return 2, nil
	}

	if index, ok := scmp.Index(oid, scmp.OID_EVENTS_EVENT_REASON); ok && index == 13579 {
		return 15, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetIndexedUint8(oid scmp.OID, index uint32) (uint8, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_EVENT) {
		if index == 13579 {
			return 6, nil
		} else {
			return 0, nil
		}
	}

	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_DOOR) {
		if index == 13579 {
			return 4, nil
		} else {
			return 0, nil
		}
	}

	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_DIRECTION) {
		if index == 13579 {
			return 2, nil
		} else {
			return 0, nil
		}
	}

	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_REASON) {
		if index == 13579 {
			return 15, nil
		} else {
			return 0, nil
		}
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetUint16(oid scmp.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetIndexedUint16(oid scmp.OID, index uint32) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetUint32(oid scmp.OID) (uint32, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_LAST) {
		return 13579, nil
	}

	if scmp.Is(oid, scmp.OID_EVENTS_INDEX) {
		return e.index, nil
	}

	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_CARD) {
		if index, ok := scmp.Index(oid, scmp.OID_EVENTS_EVENT_CARD); ok && index == 13579 {
			return 10058400, nil
		}
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetIndexedUint32(oid scmp.OID, index uint32) (uint32, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_CARD) {
		if index == 13579 {
			return 10058400, nil
		} else {
			return 0, nil
		}
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetIndexedRecord(oid scmp.OID, index uint32) (any, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetBool(oid scmp.OID) (bool, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_GRANTED) {
		if index, ok := scmp.Index(oid, scmp.OID_EVENTS_EVENT_GRANTED); ok && index == 13579 {
			return true, nil
		}
	}

	return false, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetIndexedBool(oid scmp.OID, index uint32) (bool, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_GRANTED) {
		if index == 13579 {
			return true, nil
		} else {
			return false, nil
		}
	}

	return false, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetString(oid scmp.OID) (string, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_TIMESTAMP) {
		if index, ok := scmp.Index(oid, scmp.OID_EVENTS_EVENT_TIMESTAMP); ok && index == 13579 {
			return "2024-12-17 15:52:45", nil
		}
	}

	return "", fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) GetIndexedString(oid scmp.OID, index uint32) (string, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_EVENT_TIMESTAMP) {
		if index == 13579 {
			return "2024-12-17 15:52:45", nil
		} else {
			return "", nil
		}
	}

	return "", fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) SetBool(oid scmp.OID, val bool) (bool, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_RECORD_ALL) {
		e.recordAll = val

		return e.recordAll, nil
	}

	return false, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) SetUint32A(oid scmp.OID, value []uint32) ([]uint32, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) SetIndexedRecord(oid scmp.OID, index uint32, value any) (any, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) DeleteIndexedRecord(oid scmp.OID, index uint32) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetOctets(oid scmp.OID) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (e Events) GetIndexedOctets(oid scmp.OID, index uint32) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) SetUint8(oid scmp.OID, val uint8) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (e *Events) SetUint32(oid scmp.OID, val uint32) (uint32, error) {
	if scmp.Is(oid, scmp.OID_EVENTS_INDEX) {
		e.index = val
		return e.index, nil
	}

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
