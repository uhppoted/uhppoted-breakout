package scmp

import (
	"fmt"
	"slices"
	"strings"
)

type OID []uint32

var events = []OID{
	OID_EVENTS_EVENT_EVENT,
	OID_EVENTS_EVENT_TIMESTAMP,
	OID_EVENTS_EVENT_DOOR,
	OID_EVENTS_EVENT_DIRECTION,
	OID_EVENTS_EVENT_CARD,
	OID_EVENTS_EVENT_GRANTED,
	OID_EVENTS_EVENT_REASON,
}

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
	for _, e := range events {
		if slices.Equal(t, e) {
			return slices.Equal(oid[:8], OID_EVENTS_EVENT) && slices.Equal(oid[9:], t[9:])
		}
	}

	return slices.Equal(oid, t)
}

func Index(oid OID, t OID) (uint32, bool) {
	for _, e := range events {
		if slices.Equal(t, e) && slices.Equal(oid[:8], OID_EVENTS_EVENT) && slices.Equal(oid[9:], t[9:]) {
			return oid[8], true
		}
	}

	return 0, false
}
