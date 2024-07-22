package main

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/pkg/term"
)

const VERSION = "v0.0"
const USB = "/dev/tty.usbmodem14201"

func main() {
	fmt.Printf("uhppoted-breakout::emulator %v\n\n", VERSION)

	go func() {
		for {
			if err := read(); err != nil {
				fmt.Printf("  *** ERROR %v\n", err)
			}

			time.Sleep(1 * time.Second)
		}
	}()

	interrupt := make(chan os.Signal, 1)

	signal.Notify(interrupt, syscall.SIGINT, syscall.SIGTERM)

	<-interrupt
	fmt.Printf("  ... interrupted\n")
}

func read() error {
	if t, err := term.Open(USB, term.Speed(115200), term.RawMode); err != nil {
		return err
	} else {
		println("  ... connected")

		defer func() {
			if err := t.Close(); err != nil {
				fmt.Printf("  *** ERROR %v\n", err)
			}
		}()

		if err := t.SetRaw(); err != nil {
			return err
		}

		buffer := make([]byte, 256)

		for {
			if N, err := t.Read(buffer); err != nil {
				return err
			} else if N == 0 {
				fmt.Printf("  READ %v bytes\n", N)
			} else {
				fmt.Printf("  READ %v", string(buffer[0:N]))
			}
		}
	}
}
