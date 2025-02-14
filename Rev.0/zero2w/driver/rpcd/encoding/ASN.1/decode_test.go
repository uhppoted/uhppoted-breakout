package BER

import (
	"reflect"
	"testing"
)

func TestDecodeWithEmptyMessage(t *testing.T) {
	packet := []byte{}

	if v, err := Decode(packet); err != nil {
		t.Errorf("error decoding packet (%v)", err)
	} else if v != nil {
		t.Errorf("incorrectly decoded packet\n   expected:%v\n   got:    %v", nil, v)
	}
}

func TestDecodeGetRequest(t *testing.T) {
	packet := []byte{
		48, 41, // SEQUENCE
		2, 1, 0, // INTEGER 0
		4, 6, 112, 117, 98, 108, 105, 99, // STRING  public
		160, 28, // PDU     GET
		2, 2, 48, 57, // INTEGER
		2, 1, 0, // INTEGER error:0
		2, 1, 0, // INTEGER error index:0
		48, 16, // SEQUENCE
		48, 14, // SEQUENCE
		6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1, // OID "0.1.3.6.1.4.1.65536.2.1"
		5, 0, // NULL
	}

	expected := GetRequest{
		Version:    0,
		Community:  "public",
		RequestID:  12345,
		Error:      0,
		ErrorIndex: 0,
		OID:        OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1},
	}

	if rq, err := Decode(packet); err != nil {
		t.Errorf("error decoding GET request (%v)", err)
	} else if !reflect.DeepEqual(rq, expected) {
		t.Errorf("incorrectly decoded GET request\n   expected:%#v\n   got:     %#v", expected, rq)
	}
}
