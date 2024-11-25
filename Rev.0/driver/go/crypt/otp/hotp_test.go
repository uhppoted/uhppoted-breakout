package otp

import (
	"testing"
)

func TestGenerateHOTP(t *testing.T) {
	tests := []struct {
		secret   string
		counter  uint64
		expected string
	}{
		{"A3WYPV4VWQLL6VFZ", 0, "363589"},
		{"A3WYPV4VWQLL6VFZ", 1, "848788"},
		{"A3WYPV4VWQLL6VFZ", 2, "931192"},
		{"A3WYPV4VWQLL6VFZ", 3, "038120"},
		{"A3WYPV4VWQLL6VFZ", 4, "881319"},
		{"A3WYPV4VWQLL6VFZ", 5, "797611"},
		{"A3WYPV4VWQLL6VFZ", 6, "280652"},
		{"A3WYPV4VWQLL6VFZ", 7, "442564"},
		{"A3WYPV4VWQLL6VFZ", 8, "768861"},
		{"A3WYPV4VWQLL6VFZ", 9, "947109"},
		{"A3WYPV4VWQLL6VFZ", 10, "526974"},
		{"A3WYPV4VWQLL6VFZ", 1000, "435988"},
		{"A3WYPV4VWQLL6VFZ", 10000, "830127"},
		{"A3WYPV4VWQLL6VFZ", 100000, "224802"},
		{"A3WYPV4VWQLL6VFZ", 1000000, "902124"},
		{"A3WYPV4VWQLL6VFZ", 10000000, "256869"},
		{"A3WYPV4VWQLL6VFZ", 100000000, "718655"},
		{"A3WYPV4VWQLL6VFZ", 405419896, "244053"},
	}

	for _, test := range tests {
		secret := test.secret
		counter := test.counter
		expected := test.expected

		if code, err := GenerateHOTP(secret, counter); err != nil {
			t.Fatalf("%v", err)
		} else if code != expected {
			t.Errorf("invalid code for counter %v - expected:%v, got:%v", counter, expected, code)
		}
	}
}
