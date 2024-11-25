package main

import (
	"fmt"

	"emulator/encoding/SSMP"
	"emulator/encoding/bisync"
)

func main() {
	fmt.Printf("SSMP encoder/decoder\n")
	fmt.Println()

	packet := SSMP.GetPacket{
		Community: "public",
		RequestID: 13,
		OID:       ".1.3.6.1.4.1.65536.2.1",
	}

	codec := bisync.NewBisync()

	if bytes, err := packet.Encode(); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else if encoded, err := codec.Encode(nil, bytes); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("GET       %v\n", bytes)
		fmt.Printf("GET       %v\n", encoded)
	}

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
}
