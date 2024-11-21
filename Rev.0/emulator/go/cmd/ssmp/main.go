package main

import (
	"fmt"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/go/encoding/SSMP"
	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/go/encoding/bisync"
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
		fmt.Printf("GET     %v\n", encoded)
	}

	integer := []uint8{2, 1, 13}
	if encoded, err := codec.Encode(nil, integer); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("INTEGER %v\n", encoded)
	}

	null := []uint8{5, 0}
	if encoded, err := codec.Encode(nil, null); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("NULL    %v\n", encoded)
	}

	octets := []uint8{4, 6, 'p', 'u', 'b', 'l', 'i', 'c'}
	if encoded, err := codec.Encode(nil, octets); err != nil {
		fmt.Printf("  *** ERROR %v\n", err)
	} else {
		fmt.Printf("OCTETS  %v\n", encoded)
	}
}
