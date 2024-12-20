package scmp

import (
	"fmt"
	"slices"
	"strings"
)

type OID []uint32

func (o OID) String() string {
	var b strings.Builder

	if len(o) > 0 && o[0] == 0 {
		for _, v := range o[1:] {
			fmt.Fprintf(&b, ".%v", v)
		}
	} else {
		for _, v := range o {
			fmt.Fprintf(&b, ".%v", v)
		}
	}

	return b.String()
}

func Is(oid OID, t OID) bool {
	if slices.Equal(t, OID_EVENTS_EVENT_EVENT) {
		return slices.Equal(oid[:8], t[:8]) && slices.Equal(oid[9:], t[9:])
	}

	if slices.Equal(t, OID_EVENTS_EVENT_TIMESTAMP) {
		return slices.Equal(oid[:8], t[:8]) && slices.Equal(oid[9:], t[9:])
	}

	if slices.Equal(t, OID_EVENTS_EVENT_DOOR) {
		return slices.Equal(oid[:8], t[:8]) && slices.Equal(oid[9:], t[9:])
	}

	if slices.Equal(t, OID_EVENTS_EVENT_DIRECTION) {
		return slices.Equal(oid[:8], t[:8]) && slices.Equal(oid[9:], t[9:])
	}

	if slices.Equal(t, OID_EVENTS_EVENT_CARD) {
		return slices.Equal(oid[:8], t[:8]) && slices.Equal(oid[9:], t[9:])
	}

	if slices.Equal(t, OID_EVENTS_EVENT_GRANTED) {
		return slices.Equal(oid[:8], t[:8]) && slices.Equal(oid[9:], t[9:])
	}

	if slices.Equal(t, OID_EVENTS_EVENT_REASON) {
		return slices.Equal(oid[:8], t[:8]) && slices.Equal(oid[9:], t[9:])
	}

	return slices.Equal(oid, t)
}

func Indexed(oid OID, index uint32) OID {
	if Is(oid, OID_EVENTS_EVENT_EVENT) {
		v := slices.Clone(OID_EVENTS_EVENT_EVENT)
		v[8] = index

		return v
	}

	if Is(oid, OID_EVENTS_EVENT_GRANTED) {
		v := slices.Clone(OID_EVENTS_EVENT_GRANTED)
		v[8] = index

		return v
	}

	if Is(oid, OID_EVENTS_EVENT_DOOR) {
		v := slices.Clone(OID_EVENTS_EVENT_DOOR)
		v[8] = index

		return v
	}

	if Is(oid, OID_EVENTS_EVENT_DIRECTION) {
		v := slices.Clone(OID_EVENTS_EVENT_DIRECTION)
		v[8] = index

		return v
	}

	if Is(oid, OID_EVENTS_EVENT_CARD) {
		v := slices.Clone(OID_EVENTS_EVENT_CARD)
		v[8] = index

		return v
	}

	if Is(oid, OID_EVENTS_EVENT_TIMESTAMP) {
		v := slices.Clone(OID_EVENTS_EVENT_TIMESTAMP)
		v[8] = index

		return v
	}

	if Is(oid, OID_EVENTS_EVENT_REASON) {
		v := slices.Clone(OID_EVENTS_EVENT_REASON)
		v[8] = index

		return v
	}

	panic("invalid index OID")
}

func Index(oid OID) uint32 {
	if Is(oid, OID_EVENTS_EVENT_EVENT) {
		return oid[8]
	}

	if Is(oid, OID_EVENTS_EVENT_GRANTED) {
		return oid[8]
	}

	if Is(oid, OID_EVENTS_EVENT_DOOR) {
		return oid[8]
	}

	if Is(oid, OID_EVENTS_EVENT_DIRECTION) {
		return oid[8]
	}

	if Is(oid, OID_EVENTS_EVENT_CARD) {
		return oid[8]
	}

	if Is(oid, OID_EVENTS_EVENT_TIMESTAMP) {
		return oid[8]
	}

	if Is(oid, OID_EVENTS_EVENT_REASON) {
		return oid[8]
	}

	panic("invalid OID index")
}
