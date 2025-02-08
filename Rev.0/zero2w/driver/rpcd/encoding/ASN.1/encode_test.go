package BER

import (
	"slices"
	"testing"
)

func TestEncodeGetRequest(t *testing.T) {
	rq := GetRequest{
		Version:    0,
		Community:  "public",
		RequestID:  12345,
		Error:      0,
		ErrorIndex: 0,
		OID:        OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1},
	}

	// 48 41                                       SEQUENCE
	//    2 1 0                                    INTEGER 0
	//    4 6 112 117 98 108 105 99                STRING  public
	//    160 28                                   PDU     GET
	//        2 2 48 57                            INTEGER
	//        2 1 0                                INTEGER error:0
	//        2 1 0                                INTEGER error index:0
	//        48 16                                SEQUENCE
	//           48 14                             SEQUENCE
	//              6 10 43 6 1 4 1 132 128 0 2 1  OID ".1.3.6.1.4.1.65536.2.1"
	//              5 0                            NULL
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

	if encoded, err := Encode(rq); err != nil {
		t.Errorf("error encoding GET request (%v)", err)
	} else if !slices.Equal(encoded, expected) {
		t.Errorf("incorrectly encoded GET request:\n   expected:%v\n   got:     %v", expected, encoded)
	}
}
