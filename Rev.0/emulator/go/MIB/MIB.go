package MIB

import (
	"slices"
)

var OID_CONTROLLER_ID = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 1}
var OID_CONTROLLER_VERSION = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 2}
var OID_CONTROLLER_RELEASED = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 3}
var OID_CONTROLLER_ADDRESS = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 4}
var OID_CONTROLLER_NETMASK = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 5}
var OID_CONTROLLER_GATEWAY = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 6}
var OID_CONTROLLER_MAC = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 7}
var OID_CONTROLLER_DATETIME = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 8}
var OID_CONTROLLER_ERROR = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 9}
var OID_CONTROLLER_SPECIAL_INFO = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 10}
var OID_CONTROLLER_SEQUENCE_NUMBER = []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 11}

var OID_DOORS_1_MODE = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 1, 1}
var OID_DOORS_1_DELAY = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 1, 2}
var OID_DOORS_1_UNLOCKED = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 1, 3}
var OID_DOORS_1_OPEN = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 1, 4}
var OID_DOORS_1_BUTTON = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 1, 5}

var OID_DOORS_2_MODE = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 2, 1}
var OID_DOORS_2_DELAY = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 2, 2}
var OID_DOORS_2_UNLOCKED = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 2, 3}
var OID_DOORS_2_OPEN = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 2, 4}
var OID_DOORS_2_BUTTON = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 2, 5}

var OID_DOORS_3_MODE = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 3, 1}
var OID_DOORS_3_DELAY = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 3, 2}
var OID_DOORS_3_UNLOCKED = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 3, 3}
var OID_DOORS_3_OPEN = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 3, 4}
var OID_DOORS_3_BUTTON = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 3, 5}

var OID_DOORS_4_MODE = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 4, 1}
var OID_DOORS_4_DELAY = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 4, 2}
var OID_DOORS_4_UNLOCKED = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 4, 3}
var OID_DOORS_4_OPEN = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 4, 4}
var OID_DOORS_4_BUTTON = []uint32{1, 3, 6, 1, 4, 1, 65536, 3, 4, 5}

var OID_INPUTS_TAMPER_DETECT = []uint32{1, 3, 6, 1, 4, 1, 65536, 4, 1}
var OID_INPUTS_FIRE_ALARM = []uint32{1, 3, 6, 1, 4, 1, 65536, 4, 2}

var OID_EVENTS_LISTENER = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 1}
var OID_EVENTS_INTERVAL = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 2}
var OID_EVENTS_CURRENT = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 0, 1}

var OID_EVENTS_EVENT_EVENT = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 1}
var OID_EVENTS_EVENT_GRANTED = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 2}
var OID_EVENTS_EVENT_DOOR = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 3}
var OID_EVENTS_EVENT_DIRECTION = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 4}
var OID_EVENTS_EVENT_CARD = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 5}
var OID_EVENTS_EVENT_TIMESTAMP = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 6}
var OID_EVENTS_EVENT_REASON = []uint32{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 7}

func Indexed(oid OID, index uint32) OID {
	if OID.Equal(oid, OID_EVENTS_EVENT_EVENT) {
		v := slices.Clone(OID_EVENTS_EVENT_EVENT)
		v[8] = index

		return v
	}

	if OID.Equal(oid, OID_EVENTS_EVENT_GRANTED) {
		v := slices.Clone(OID_EVENTS_EVENT_GRANTED)
		v[8] = index

		return v
	}

	if OID.Equal(oid, OID_EVENTS_EVENT_DOOR) {
		v := slices.Clone(OID_EVENTS_EVENT_DOOR)
		v[8] = index

		return v
	}

	if OID.Equal(oid, OID_EVENTS_EVENT_DIRECTION) {
		v := slices.Clone(OID_EVENTS_EVENT_DIRECTION)
		v[8] = index

		return v
	}

	if OID.Equal(oid, OID_EVENTS_EVENT_CARD) {
		v := slices.Clone(OID_EVENTS_EVENT_CARD)
		v[8] = index

		return v
	}

	if OID.Equal(oid, OID_EVENTS_EVENT_TIMESTAMP) {
		v := slices.Clone(OID_EVENTS_EVENT_TIMESTAMP)
		v[8] = index

		return v
	}

	if OID.Equal(oid, OID_EVENTS_EVENT_REASON) {
		v := slices.Clone(OID_EVENTS_EVENT_REASON)
		v[8] = index

		return v
	}

	panic("invalid index OID")
}

func Index(oid OID, t OID) uint32 {
	if len(oid) == len(OID_EVENTS_EVENT_EVENT) && slices.Equal(oid[:8], OID_EVENTS_EVENT_EVENT[:8]) {
		if slices.Equal(oid[9:], OID_EVENTS_EVENT_EVENT[9:]) {
			return oid[8]
		}

		if slices.Equal(oid[9:], OID_EVENTS_EVENT_GRANTED[9:]) {
			return oid[8]
		}

		if slices.Equal(oid[9:], OID_EVENTS_EVENT_DOOR[9:]) {
			return oid[8]
		}

		if slices.Equal(oid[9:], OID_EVENTS_EVENT_DIRECTION[9:]) {
			return oid[8]
		}

		if slices.Equal(oid[9:], OID_EVENTS_EVENT_CARD[9:]) {
			return oid[8]
		}

		if slices.Equal(oid[9:], OID_EVENTS_EVENT_TIMESTAMP[9:]) {
			return oid[8]
		}

		if slices.Equal(oid[9:], OID_EVENTS_EVENT_REASON[9:]) {
			return oid[8]
		}
	}

	panic("invalid OID index")
}
