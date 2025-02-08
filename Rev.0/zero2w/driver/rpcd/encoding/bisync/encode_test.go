package bisync

import (
	"slices"
	"testing"
)

func TestEncodeEmptyPacket(t *testing.T) {
	packet := []byte{}
	expected := []byte{22, 22, 2, 3, 190, 248}

	if bytes, err := Encode(nil, packet); err != nil {
		t.Errorf("error encoding empty packet (%v)", err)
	} else if !slices.Equal(bytes, expected) {
		t.Errorf("incorrectly encoded packet:\n   expected:%#v\n   got:     %#v", expected, bytes)
	}
}
