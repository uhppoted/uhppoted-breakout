package events

import (
	"errors"
	"fmt"
	"sync/atomic"

	"emulator/db"
	"emulator/driver/rpcd"
	"emulator/entities"
	"emulator/log"
	"emulator/scmp"
)

const LOGTAG = "EVENTS"

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

func (e *Events) Add(event rpcd.Event) entities.Event {
	evt := entities.Event{
		Index:     0,
		Type:      lookup(event.Var.OID),
		Granted:   false,
		Door:      door(event.Var.OID),
		Direction: 0,
		Card:      0,
		Timestamp: event.Timestamp,
		Reason:    reason(event.Var.OID, event.Var.Value),
	}

	if index, err := db.PutEvent(evt); err != nil {
		warnf("%v", err)
	} else {
		evt.Index = index
	}

	return evt
}

func (e *Events) Get(index uint32) (entities.Event, error) {
	get := func(ix uint32) (entities.Event, error) {
		if record, err := db.GetEvent(ix); err != nil {
			return entities.Event{}, err
		} else {
			return record, nil
		}
	}

	if record, err := get(index); err != nil && !errors.Is(err, entities.ErrRecordNotFound) {
		return entities.Event{}, err
	} else if err == nil {
		return record, nil
	} else if first, last, err := db.GetEvents(); err != nil {
		return entities.Event{}, err
	} else if index == 0 {
		return get(first)
	} else if index == 0xffffffff {
		return get(last)
	} else if index < first {
		return entities.Event{
			Index: index,
			Type:  0xff, // overwritten
		}, nil
	} else if index > last {
		return get(last)
	} else {
		return entities.Event{}, entities.ErrRecordNotFound
	}
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
	log.Debugf(LOGTAG, format, args...)
}

func infof(format string, args ...any) {
	log.Infof(LOGTAG, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(LOGTAG, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(LOGTAG, format, args...)
}
