package BER

import (
	"reflect"
	"testing"
)

func TestUnpackEmptyMessage(t *testing.T) {
	bytes := []byte{}

	if packet, _, err := unpack(bytes); err != nil {
		t.Errorf("error unpacked packet (%v)", err)
	} else if packet != nil {
		t.Errorf("incorrectly unpacked packet\n   expected:%v\n   got:    %v", nil, packet)
	}
}

// 48 41                                     SEQUENCE
//
//	2 1 0                                    INTEGER 0
//	4 6 112 117 98 108 105 99                STRING  public
//	160 28                                   PDU     GET
//	    2 2 48 57                            INTEGER
//	    2 1 0                                INTEGER error:0
//	    2 1 0                                INTEGER error index:0
//	    48 16                                SEQUENCE
//	       48 14                             SEQUENCE
//	          6 10 43 6 1 4 1 132 128 0 2 1  OID "0.1.3.6.1.4.1.65536.2.1"
//	          5 0                            NULL
func TestUnpackGetRequest(t *testing.T) {
	bytes := []byte{
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

	expected := sequence{
		int64(0),
		"public",
		pdu{
			int64(12345),
			int64(0),
			int64(0),
			sequence{
				sequence{
					OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1},
					null{},
				},
			},
		},
	}

	if packet, _, err := unpack(bytes); err != nil {
		t.Errorf("error unpacking packet (%v)", err)
	} else if seq, ok := packet.(sequence); !ok {
		t.Errorf("incorrectly unpacking packet\n   expected:%#v\n   got:     %#v", expected, packet)
	} else if len(seq) != len(expected) {
		t.Errorf("incorrectly unpacking packet\n   expected:%#v\n   got:     %#v", expected, packet)
	} else {
		for i, v := range seq {
			if !reflect.DeepEqual(v, expected[i]) {
				t.Errorf("incorrectly unpacking packet element %v\n   expected:%v (%T)\n   got:     %v (%T)",
					i,
					expected[i], expected[i],
					v, v)
			}
		}
	}
}
