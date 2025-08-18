package BER

import (
	"slices"
	"testing"
)

func TestEncodeGetRequest(t *testing.T) {
	rq := GetRequest{
		Version:   0,
		Community: "public",
		RequestID: 12345,
		OID:       OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1},
	}

	// 48 41                                       SEQUENCE
	//    2 1 0                                    INTEGER 0
	//    4 6 112 117 98 108 105 99                STRING  public
	//    160 28                                   PDU     GET-REQUEST
	//        2 2 48 57                            INTEGER
	//        2 1 0                                INTEGER error:0
	//        2 1 0                                INTEGER error index:0
	//        48 16                                SEQUENCE
	//           48 14                             SEQUENCE
	//              6 10 43 6 1 4 1 132 128 0 2 1  OID "0.1.3.6.1.4.1.65536.2.1"
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

	if encoded, err := EncodeGetRequest(rq); err != nil {
		t.Errorf("error encoding GET request (%v)", err)
	} else if !slices.Equal(encoded, expected) {
		t.Errorf("incorrectly encoded GET request:\n   expected:%v\n   got:     %v", expected, encoded)
	}
}

// 48 41                                     SEQUENCE
//
//	2 1 0                                    INTEGER 0
//	4 6 112 117 98 108 105 99                STRING  public
//	162 28                                   PDU     GET-RESPONSE
//	    2 2 48 57                            INTEGER
//	    2 1 0                                INTEGER error:0
//	    2 1 0                                INTEGER error index:0
//	    48 16                                SEQUENCE
//	       48 14                             SEQUENCE
//	          6 10 43 6 1 4 1 132 128 0 2 1  OID "0.1.3.6.1.4.1.65536.2.1"
//	          2 4 24 42 55 120               INTEGER 405419896
func TestEncodeGetResponse(t *testing.T) {
	response := GetResponse{
		Version:    0,
		Community:  "public",
		RequestID:  12345,
		Error:      0,
		ErrorIndex: 0,
		OID:        OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 1},
		Value:      uint32(405419896),
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

	if encoded, err := EncodeGetResponse(response); err != nil {
		t.Errorf("error encoding GET response (%v)", err)
	} else if !slices.Equal(encoded, expected) {
		t.Errorf("incorrectly encoded GET response:\n   expected:%v\n   got:     %v", expected, encoded)
	}
}

// 48 60                                     SEQUENCE
//
//	2 1 0                                    INTEGER 0
//	4 6 112 117 98 108 105 99                STRING  public
//	163 47                                   PDU     SET-REQUEST
//	    2 2 48 57                            INTEGER
//	    2 1 0                                INTEGER error:0
//	    2 1 0                                INTEGER error index:0
//	    48 35                                SEQUENCE
//	       48 33                             SEQUENCE
//	          6 10 43 6 1 4 1 132 128 0 2 1  OID "0.1.3.6.1.4.1.65536.2.1"
//	          4 19 "2025-02-26 14:13:48"     STRING "2025-02-26 14:13:48"
func TestEncodeSetRequest(t *testing.T) {
	rq := SetRequest{
		Version:   0,
		Community: "public",
		RequestID: 12345,
		OID:       OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 8},
		Value:     "2025-02-26 14:13:48",
	}

	expected := []byte{
		48, 60,
		2, 1, 0,
		4, 6, 112, 117, 98, 108, 105, 99,
		163, 47,
		2, 2, 48, 57,
		2, 1, 0,
		2, 1, 0,
		48, 35,
		48, 33,
		6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 8,
		4, 19, 50, 48, 50, 53, 45, 48, 50, 45, 50, 54, 32, 49, 52, 58, 49, 51, 58, 52, 56,
	}

	if encoded, err := EncodeSetRequest(rq); err != nil {
		t.Errorf("error encoding SET request (%v)", err)
	} else if !slices.Equal(encoded, expected) {
		t.Errorf("incorrectly encoded SET request:\n   expected:%v\n   got:     %v", expected, encoded)
	}
}

// 48 60                                     SEQUENCE
//
//	2 1 0                                    INTEGER 0
//	4 6 112 117 98 108 105 99                STRING  public
//	163 47                                   PDU     SET-REQUEST
//	    2 2 48 57                            INTEGER
//	    2 1 0                                INTEGER error:0
//	    2 1 0                                INTEGER error index:0
//	    48 35                                SEQUENCE
//	       48 33                             SEQUENCE
//	          6 10 43 6 1 4 1 132 128 0 2 1  OID "0.1.3.6.1.4.1.65536.2.1"
//	          1 1 1                          BOOLEAN true
func TestEncodeSetBoolRequest(t *testing.T) {
	rq := SetRequest{
		Version:   0,
		Community: "public",
		RequestID: 12345,
		OID:       OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 8},
		Value:     true,
	}

	expected := []byte{
		48, 42,
		2, 1, 0,
		4, 6, 112, 117, 98, 108, 105, 99,
		163, 29,
		2, 2, 48, 57,
		2, 1, 0,
		2, 1, 0,
		48, 17,
		48, 15,
		6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 8,
		1, 1, 1,
	}

	if encoded, err := EncodeSetRequest(rq); err != nil {
		t.Errorf("error encoding SET request (%v)", err)
	} else if !slices.Equal(encoded, expected) {
		t.Errorf("incorrectly encoded SET request:\n   expected:%v\n   got:     %v", expected, encoded)
	}
}

// 48 60                                     SEQUENCE
//
//	2 1 0                                    INTEGER 0
//	4 6 112 117 98 108 105 99                STRING  public
//	163 47                                   PDU     SET-REQUEST
//	    2 2 48 57                            INTEGER
//	    2 1 0                                INTEGER error:0
//	    2 1 0                                INTEGER error index:0
//	    48 35                                SEQUENCE
//	       48 33                             SEQUENCE
//	          6 10 43 6 1 4 1 132 128 0 2 1  OID "0.1.3.6.1.4.1.65536.2.1"
//	          2 1 3                          INTEGER 3
func TestEncodeSetUint8Request(t *testing.T) {
	rq := SetRequest{
		Version:   0,
		Community: "public",
		RequestID: 12345,
		OID:       OID{0, 1, 3, 6, 1, 4, 1, 65536, 2, 8},
		Value:     uint8(3),
	}

	expected := []byte{
		48, 42,
		2, 1, 0,
		4, 6, 112, 117, 98, 108, 105, 99,
		163, 29,
		2, 2, 48, 57,
		2, 1, 0,
		2, 1, 0,
		48, 17,
		48, 15,
		6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 8,
		2, 1, 3,
	}

	if encoded, err := EncodeSetRequest(rq); err != nil {
		t.Errorf("error encoding SET request (%v)", err)
	} else if !slices.Equal(encoded, expected) {
		t.Errorf("incorrectly encoded SET request:\n   expected:%v\n   got:     %v", expected, encoded)
	}
}
