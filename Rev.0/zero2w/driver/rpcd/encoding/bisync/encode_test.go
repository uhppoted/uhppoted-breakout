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

func TestEncodeGetRequest(t *testing.T) {
	packet := []byte{
		48, 41,
		2, 1, 0,
		4, 6, 112, 117, 98, 108, 105, 99,
		160, 28,
		2, 2, 48, 57,
		2, 1, 0,
		2, 1, 0,
		48, 16,
		48, 14,
		6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1,
		5, 0,
	}

	expected := []byte{
		22, 22, 2, 48, 41, 16, 2, 16, 1, 0, 4, 16, 6, 112, 117,
		98, 108, 105, 99, 160, 28, 16, 2, 16, 2, 48, 57, 16, 2, 16,
		1, 0, 16, 2, 16, 1, 0, 48, 16, 16, 48, 14, 16, 6, 10,
		43, 16, 6, 16, 1, 4, 16, 1, 132, 128, 0, 16, 2, 16, 1,
		16, 5, 0, 3, 135, 41,
	}

	if encoded, err := Encode(nil, packet); err != nil {
		t.Fatalf("error encoding packet (%v)", err)
	} else if !slices.Equal(encoded, expected) {
		t.Errorf("incorrectly encoded packet\n   expected:%v\n   got:     %v", expected, encoded)
	} else {
	}
}

func TestEncodeGetResponse(t *testing.T) {
	packet := []byte{
		48, 45,
		2, 1, 0,
		4, 6, 112, 117, 98, 108, 105, 99,
		162, 32,
		2, 2, 48, 57,
		2, 1, 0,
		2, 1, 0,
		48, 20,
		48, 18,
		6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1,
		2, 4, 24, 42, 55, 120,
	}

	expected := []byte{
		22, 22, 2, 48, 45, 16, 2, 16, 1, 0, 4, 16, 6, 112, 117,
		98, 108, 105, 99, 162, 32, 16, 2, 16, 2, 48, 57, 16, 2, 16,
		1, 0, 16, 2, 16, 1, 0, 48, 20, 48, 18, 16, 6, 10, 43,
		16, 6, 16, 1, 4, 16, 1, 132, 128, 0, 16, 2, 16, 1, 16,
		2, 4, 24, 42, 55, 120, 3, 166, 151,
	}

	if encoded, err := Encode(nil, packet); err != nil {
		t.Fatalf("error encoding packet (%v)", err)
	} else if !slices.Equal(encoded, expected) {
		t.Errorf("incorrectly encoded packet\n   expected:%v\n   got:     %v", expected, encoded)
	} else {
	}
}
