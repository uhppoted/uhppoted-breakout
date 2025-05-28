package events

import (
	"emulator/entities"
	"emulator/scmp"
)

func Lookup(OID string) entities.EventType {
	if oid, err := scmp.Parse(OID); err == nil {
		doors := []scmp.OID{
			scmp.OID_DOORS_1_OPEN, scmp.OID_DOORS_1_BUTTON, scmp.OID_DOORS_1_UNLOCKED,
			scmp.OID_DOORS_2_OPEN, scmp.OID_DOORS_2_BUTTON, scmp.OID_DOORS_2_UNLOCKED,
			scmp.OID_DOORS_3_OPEN, scmp.OID_DOORS_3_BUTTON, scmp.OID_DOORS_3_UNLOCKED,
			scmp.OID_DOORS_4_OPEN, scmp.OID_DOORS_4_BUTTON, scmp.OID_DOORS_4_UNLOCKED,
		}

		for _, v := range doors {
			if scmp.Equal(oid, v) {
				return entities.EventDoor
			}
		}
	}

	return entities.EventUnknown
}

func Door(OID string) uint8 {
	if oid, err := scmp.Parse(OID); err == nil {
		doors := map[uint8][]scmp.OID{
			1: {scmp.OID_DOORS_1_OPEN, scmp.OID_DOORS_1_BUTTON, scmp.OID_DOORS_1_UNLOCKED},
			2: {scmp.OID_DOORS_2_OPEN, scmp.OID_DOORS_2_BUTTON, scmp.OID_DOORS_2_UNLOCKED},
			3: {scmp.OID_DOORS_3_OPEN, scmp.OID_DOORS_3_BUTTON, scmp.OID_DOORS_3_UNLOCKED},
			4: {scmp.OID_DOORS_4_OPEN, scmp.OID_DOORS_4_BUTTON, scmp.OID_DOORS_4_UNLOCKED},
		}

		for k, v := range doors {
			for _, u := range v {
				if scmp.Equal(oid, u) {
					return k
				}
			}
		}
	}

	return 0
}

func Reason(OID string, value any) entities.EventReason {
	if oid, err := scmp.Parse(OID); err == nil {
		open := []scmp.OID{
			scmp.OID_DOORS_1_OPEN,
			scmp.OID_DOORS_2_OPEN,
			scmp.OID_DOORS_3_OPEN,
			scmp.OID_DOORS_4_OPEN,
		}

		for _, v := range open {
			if scmp.Equal(oid, v) {
				if b, ok := value.(bool); ok {
					if b {
						return entities.ReasonDoorOpen
					} else {
						return entities.ReasonDoorClosed
					}
				}
			}
		}
	}

	return entities.ReasonUnknown
}
