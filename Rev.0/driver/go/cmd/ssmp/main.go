package main

import (
	"fmt"
	"log"
	"os"

	"github.com/gosnmp/gosnmp"

	"emulator/encoding/bisync"
)

func main() {
	fmt.Printf("SSMP encoder/decoder\n")
	fmt.Println()

	codec := bisync.NewBisync()

	// packet := SSMP.GetPacket{
	// 	Community: "public",
	// 	RequestID: 13,
	// 	OID:       ".1.3.6.1.4.1.65536.2.1",
	// }

	// if bytes, err := packet.Encode(); err != nil {
	// 	fmt.Printf("  *** ERROR %v\n", err)
	// } else if encoded, err := codec.Encode(nil, bytes); err != nil {
	// 	fmt.Printf("  *** ERROR %v\n", err)
	// } else {
	// 	fmt.Printf("GET       %v\n", bytes)
	// 	fmt.Printf("GET       %v\n", encoded)
	// }

	integer := []uint8{2, 1, 13}
	if encoded, err := codec.Encode(nil, integer); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("INTEGER   %v\n", encoded)
	}

	null := []uint8{5, 0}
	if encoded, err := codec.Encode(nil, null); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("NULL      %v\n", encoded)
	}

	octets := []uint8{4, 6, 'p', 'u', 'b', 'l', 'i', 'c'}
	if encoded, err := codec.Encode(nil, octets); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("OCTETS    %v\n", encoded)
	}

	OID := []uint8{6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1}
	if encoded, err := codec.Encode(nil, OID); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("OID       %v\n", encoded)
	}

	sequence := []uint8{48, 14, 6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1, 5, 0}
	if encoded, err := codec.Encode(nil, sequence); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("SEQUENCE  %v\n", encoded)
	}

	nested := []uint8{48, 16, 48, 14, 6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1, 5, 0}
	if encoded, err := codec.Encode(nil, nested); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("SEQUENCES %v\n", encoded)
	}

	PDU := []uint8{160, 27, 2, 1, 13, 2, 1, 0, 2, 1, 0, 48, 16, 48, 14, 6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1, 5, 0}
	if encoded, err := codec.Encode(nil, PDU); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("PDU       %v\n", encoded)
	}

	{
		logger := gosnmp.NewLogger(log.New(os.Stdout, "", 0))

		pdu := gosnmp.SnmpPDU{
			Value: gosnmp.Null,
			Name:  ".1.3.6.1.4.1.65536.2.1",
			Type:  gosnmp.Null,
		}

		packet := gosnmp.SnmpPacket{
			Version:         gosnmp.Version1,
			ContextEngineID: "ssmp",
			ContextName:     "ssmp",
			Community:       "public",
			PDUType:         gosnmp.GetRequest,
			MsgID:           98764,
			RequestID:       12345,
			MsgMaxSize:      512,
			Error:           gosnmp.NoError,
			ErrorIndex:      0,
			NonRepeaters:    0,
			MaxRepetitions:  0,
			Variables:       []gosnmp.SnmpPDU{pdu},
			Logger:          logger,
		}

		GET, _ := packet.MarshalMsg()

		if encoded, err := codec.Encode(nil, GET); err != nil {
			fmt.Printf("  *** ERROR %v\n", err)
		} else {
			fmt.Printf("GET       %v\n", encoded)
		}
	}
}
