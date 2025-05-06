package scmp

import (
	"fmt"
	"regexp"
	"slices"
	"strconv"
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

func Parse(s string) (OID, error) {
	re := regexp.MustCompile(`^0?(([.]\d+)+)$`)
	m := re.FindAllStringSubmatch(s, -1)

	if m != nil {
		re := regexp.MustCompile(`[.]([0-9]+)`)
		if matches := re.FindAllStringSubmatch(m[0][1], -1); matches != nil {
			oid := OID{0}
			for _, v := range matches {
				if u, err := strconv.ParseUint(v[1], 10, 32); err != nil {
					return OID{}, fmt.Errorf("invalid OID (%v)", s)
				} else {
					oid = append(oid, uint32(u))
				}

			}
			return oid, nil
		}
	}

	return OID{}, fmt.Errorf("invalid OID (%v)", s)
}

func Equal(p OID, q OID) bool {
	if slices.Equal(p, q) {
		return true
	} else if len(p) > 0 && p[0] == 0 && slices.Equal(p[1:], q) {
		return true
	} else if len(q) > 0 && q[0] == 0 && slices.Equal(p, q[1:]) {
		return true
	}

	return false
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
