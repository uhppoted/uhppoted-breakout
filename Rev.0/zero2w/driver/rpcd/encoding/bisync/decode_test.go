package bisync

import (
	"slices"
	"testing"
	"time"
)

type callback struct {
	pipe chan []uint8
}

func (c callback) OnENQ() {
}

func (c callback) OnACK() {
}

func (c callback) OnMessage(header []uint8, content []uint8) {
	select {
	case c.pipe <- content:
	default:
	}
}

func TestDecodeEmptyPacket(t *testing.T) {
	bytes := []byte{}
	codec := NewBisync()
	f := callback{
		pipe: make(chan []uint8, 1),
	}

	if err := codec.Decode(bytes, f); err != nil {
		t.Errorf("error decoding empty packet (%v)", err)
	} else {
		select {
		case <-f.pipe:
			t.Errorf("unexpectedly decoded empty packet")
		case <-time.After(100 * time.Millisecond):
			// ok
		}
	}
}

func TestDecodeGetRequest(t *testing.T) {
	bytes := []byte{
		22, 22, 2, 48, 41, 16, 2, 16, 1, 0, 4, 16, 6, 112, 117,
		98, 108, 105, 99, 160, 28, 16, 2, 16, 2, 48, 57, 16, 2, 16,
		1, 0, 16, 2, 16, 1, 0, 48, 16, 16, 48, 14, 16, 6, 10,
		43, 16, 6, 16, 1, 4, 16, 1, 132, 128, 0, 16, 2, 16, 1,
		16, 5, 0, 3, 135, 41,
	}

	expected := []byte{
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

	codec := NewBisync()
	f := callback{
		pipe: make(chan []uint8, 1),
	}

	if err := codec.Decode(bytes, f); err != nil {
		t.Errorf("error decoding GET request (%v)", err)
	} else {
		select {
		case packet := <-f.pipe:
			if !slices.Equal(packet, expected) {
				t.Errorf("incorrectly decoded GET request:\n   expected:%#v\n   got:     %#v", expected, packet)
			}
		case <-time.After(100 * time.Millisecond):
			t.Errorf("timeout decoding GET request")
		}
	}
}

func TestDecodeGetResponse(t *testing.T) {
	bytes := []byte{
		22, 22, 2, 48, 45, 16, 2, 16, 1, 0, 4, 16, 6, 112, 117,
		98, 108, 105, 99, 162, 32, 16, 2, 16, 2, 48, 57, 16, 2, 16,
		1, 0, 16, 2, 16, 1, 0, 48, 20, 48, 18, 16, 6, 10, 43,
		16, 6, 16, 1, 4, 16, 1, 132, 128, 0, 16, 2, 16, 1, 16,
		2, 4, 24, 42, 55, 120, 3, 166, 151,
	}

	expected := []byte{
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

	codec := NewBisync()
	f := callback{
		pipe: make(chan []uint8, 1),
	}

	if err := codec.Decode(bytes, f); err != nil {
		t.Errorf("error decoding GET response (%v)", err)
	} else {
		select {
		case packet := <-f.pipe:
			if !slices.Equal(packet, expected) {
				t.Errorf("incorrectly decoded GET response:\n   expected:%#v\n   got:     %#v", expected, packet)
			}
		case <-time.After(100 * time.Millisecond):
			t.Errorf("timeout decoding GET request")
		}
	}
}

func TestDecodeBreakoutResponse(t *testing.T) {
	bytes := []byte{
		22, 22, 2, 48, 44, 16, 2, 16, 1, 0, 4, 16, 6, 112, 117,
		98, 108, 105, 99, 162, 31, 16, 2, 16, 1, 16, 1, 16, 2, 16,
		1, 0, 16, 2, 16, 1, 0, 48, 20, 48, 18, 16, 6, 10, 43,
		16, 6, 16, 1, 4, 16, 1, 132, 128, 0, 16, 2, 16, 1, 16, 2, 4, 24, 42,
	}

	expected := []byte{
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

	codec := NewBisync()
	f := callback{
		pipe: make(chan []uint8, 1),
	}

	if err := codec.Decode(bytes, f); err != nil {
		t.Errorf("error decoding GET response (%v)", err)
	} else {
		select {
		case packet := <-f.pipe:
			if !slices.Equal(packet, expected) {
				t.Errorf("incorrectly decoded GET response:\n   expected:%#v\n   got:     %#v", expected, packet)
			}
		case <-time.After(100 * time.Millisecond):
			t.Errorf("timeout decoding GET request")
		}
	}
}
