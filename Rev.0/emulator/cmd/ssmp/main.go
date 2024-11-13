package main

import (
	"fmt"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/encoding/SSMP"
	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/encoding/bisync"
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
		fmt.Printf("%v\n", encoded)
	}
}
