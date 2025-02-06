package BER

import (
	"fmt"
	"testing"
)

func TestOIDStringer(t *testing.T) {
	tests := []struct {
		oid      OID
		expected string
	}{
		{OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1}, "0.1.3.6.1.4.1.65536.2.1"},
	}

	for _, test := range tests {
		v := fmt.Sprintf("%v", test.oid)
		if v != test.expected {
			t.Errorf("incorrectly formatted OID - expected:%v, got:%v", test.expected, v)
		}
	}
}

func TestOIDEqual(t *testing.T) {
	tests := []struct {
		oid1     OID
		oid2     OID
		expected bool
	}{
		{OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1}, OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1}, true},
	}

	for _, test := range tests {
		if b := Equal(test.oid1, test.oid2); b != test.expected {
			t.Errorf("incorrect 'Equal' for OIDs %v and %v - expected:%v, got:%v", test.oid1, test.oid2, test.expected, b)
		}
	}
}

func TestParseOID(t *testing.T) {
	tests := []struct {
		oid      string
		expected OID
	}{
		{"0.1.3.6.1.4.1.65536.2.1", OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1}},
		{".1.3.6.1.4.1.65536.2.1", OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1}},
	}

	for _, test := range tests {
		if oid, err := ParseOID(test.oid); err != nil {
			t.Errorf("error parsing OID %v (%v)", test.oid, err)
		} else if !Equal(oid, test.expected) {
			t.Errorf("incorrectly parsed OID %v - expected:%v, got:%v", test.oid, test.expected, oid)
		}
	}
}
