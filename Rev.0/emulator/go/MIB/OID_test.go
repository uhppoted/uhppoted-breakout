package MIB

import (
	"fmt"
	"testing"
)

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
