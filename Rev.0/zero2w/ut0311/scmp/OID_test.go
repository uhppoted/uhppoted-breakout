package scmp

import (
	"fmt"
	"slices"
	"testing"
)

func TestOIDParse(t *testing.T) {
	t.Skip()
	// tests := []struct {
	// 	oid      string
	// 	expected OID
	// }{
	// 	{".1.3.6.1.4.1.65536.1.1", []uint32{1, 3, 6, 1, 4, 1, 65536, 1, 1}},
	// 	{".1.3.6.1.4.1.65536.1.1", []uint32{0, 1, 3, 6, 1, 4, 1, 65536, 1, 1}},
	// }
// 
	// for _, test := range tests {
	// 	if oid, err := Parse(test.oid); err != nil {
	// 		t.Errorf("error %v parsing OID %v", err, test.oid)
	// 	} else if !slices.Equal(oid, test.expected) {
	// 		t.Errorf("incorrectly parsed OID - expected:%v, got:%v", test.expected, oid)
	// 	}
	// }
}

func TestOIDEquals(t *testing.T) {
	t.Skip()
	// tests := []struct {
	// 	oid      string
	// 	expected OID
	// }{
	// 	{".1.3.6.1.4.1.65536.1.1", []uint32{1, 3, 6, 1, 4, 1, 65536, 1, 1}},
	// 	{".1.3.6.1.4.1.65536.1.1", []uint32{0, 1, 3, 6, 1, 4, 1, 65536, 1, 1}},
	// }
// 
	// for _, test := range tests {
	// 	if oid, err := Parse(test.oid); err != nil {
	// 		t.Errorf("error %v parsing OID %v", err, test.oid)
	// 	} else if !slices.Equal(oid, test.expected) {
	// 		t.Errorf("incorrectly parsed OID - expected:%v, got:%v", test.expected, oid)
	// 	}
	// }
}

func TestOIDString(t *testing.T) {
	tests := []struct {
		oid      OID
		expected string
	}{
		{[]uint32{1, 3, 6, 1, 4, 1, 65536, 1, 1}, ".1.3.6.1.4.1.65536.1.1"},
		{[]uint32{0, 1, 3, 6, 1, 4, 1, 65536, 1, 1}, ".1.3.6.1.4.1.65536.1.1"},
	}

	for _, test := range tests {
		v := fmt.Sprintf("%v", test.oid)

		if v != test.expected {
			t.Errorf("incorrect OID string - expected:%v, got:%v", test.expected, v)
		}
	}
}

func TestOIDIsEvent(t *testing.T) {
	tests := []struct {
		oid      OID
		is       OID
		expected bool
	}{
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13579, 0}, OID_EVENTS_EVENT_EVENT, false},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13578, 1}, OID_EVENTS_EVENT_EVENT, true},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13577, 2}, OID_EVENTS_EVENT_EVENT, false},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13579, 1}, OID_EVENTS_EVENT_GRANTED, false},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13578, 2}, OID_EVENTS_EVENT_GRANTED, true},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13577, 3}, OID_EVENTS_EVENT_GRANTED, false},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13579, 2}, OID_EVENTS_EVENT_DOOR, false},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13578, 3}, OID_EVENTS_EVENT_DOOR, true},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13577, 4}, OID_EVENTS_EVENT_DOOR, false},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13579, 3}, OID_EVENTS_EVENT_DIRECTION, false},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13578, 4}, OID_EVENTS_EVENT_DIRECTION, true},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13577, 5}, OID_EVENTS_EVENT_DIRECTION, false},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13579, 4}, OID_EVENTS_EVENT_CARD, false},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13578, 5}, OID_EVENTS_EVENT_CARD, true},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13577, 6}, OID_EVENTS_EVENT_CARD, false},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13579, 5}, OID_EVENTS_EVENT_TIMESTAMP, false},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13578, 6}, OID_EVENTS_EVENT_TIMESTAMP, true},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13577, 7}, OID_EVENTS_EVENT_TIMESTAMP, false},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13579, 6}, OID_EVENTS_EVENT_REASON, false},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13578, 7}, OID_EVENTS_EVENT_REASON, true},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13577, 8}, OID_EVENTS_EVENT_REASON, false},
	}

	for _, v := range tests {
		if Is(v.oid, v.is) != v.expected {
			t.Errorf("incorrect 'Is' for OID %v", v.oid)
		}
	}
}

func TestOIDIndexEvent(t *testing.T) {
	tests := []struct {
		oid   OID
		is    OID
		ok    bool
		index uint32
	}{
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13579, 0}, OID_EVENTS_EVENT_EVENT, false, 0},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13580, 1}, OID_EVENTS_EVENT_EVENT, true, 13580},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13581, 2}, OID_EVENTS_EVENT_EVENT, false, 0},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13582, 1}, OID_EVENTS_EVENT_GRANTED, false, 0},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13583, 2}, OID_EVENTS_EVENT_GRANTED, true, 13583},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13584, 3}, OID_EVENTS_EVENT_GRANTED, false, 0},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13585, 2}, OID_EVENTS_EVENT_DOOR, false, 0},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13586, 3}, OID_EVENTS_EVENT_DOOR, true, 13586},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13587, 4}, OID_EVENTS_EVENT_DOOR, false, 0},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13588, 3}, OID_EVENTS_EVENT_DIRECTION, false, 0},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13589, 4}, OID_EVENTS_EVENT_DIRECTION, true, 13589},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13590, 5}, OID_EVENTS_EVENT_DIRECTION, false, 0},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13591, 4}, OID_EVENTS_EVENT_CARD, false, 0},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13592, 5}, OID_EVENTS_EVENT_CARD, true, 13592},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13593, 6}, OID_EVENTS_EVENT_CARD, false, 0},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13594, 5}, OID_EVENTS_EVENT_TIMESTAMP, false, 0},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13595, 6}, OID_EVENTS_EVENT_TIMESTAMP, true, 13595},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13596, 7}, OID_EVENTS_EVENT_TIMESTAMP, false, 0},

		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13597, 6}, OID_EVENTS_EVENT_REASON, false, 0},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13598, 7}, OID_EVENTS_EVENT_REASON, true, 13598},
		{OID{1, 3, 6, 1, 4, 1, 65536, 6, 13599, 8}, OID_EVENTS_EVENT_REASON, false, 0},
	}

	for _, v := range tests {
		if ix, ok := Index(v.oid, v.is); ok != v.ok {
			t.Errorf("incorrect index for OID %v - expected:%v, got:%v", v.oid, v.ok, ok)
		} else if ix != v.index {
			t.Errorf("incorrect index for OID %v - expected:%v, got:%v", v.oid, v.index, ix)
		}
	}
}
