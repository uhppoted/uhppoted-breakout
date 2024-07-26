package main

import (
	"fmt"
	"os"
	"os/signal"
	"slices"
	"syscall"
	"time"

	"github.com/pkg/term"
)

const VERSION = "v0.0"
const USB = "/dev/tty.usbmodem14201"
const SYN byte = 22
const ENQ byte = 5
const ACK byte = 6

var SYN_SYN_ACK = []byte{SYN, SYN, ACK}

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

		// NTS: term.Close hangs indefinitely if a read is pending
		//      (seems happy enough to reopen the connection in any event)
		// defer func() {
		// 	if err := t.Close(); err != nil {
		// 		fmt.Printf("  *** ERROR %v\n", err)
		// 	}
		// }()

		if err := t.SetRaw(); err != nil {
			return err
		}

		// ... read
		pipe := make(chan []byte)
		errors := make(chan error)

		go func() {
			buffer := make([]byte, 256)

			for {
				if N, err := t.Read(buffer); err != nil {
					errors <- err
					return
				} else if N > 0 {
					pipe <- buffer[0:N]
				}
			}
		}()

		// ... send SYN-SYN-ENQ until acknowledged
	enq:
		for {
			cmd := []byte{SYN, SYN, ENQ}
			if N, err := t.Write(cmd); err != nil {
				return err
			} else if N != len(cmd) {
				return fmt.Errorf("error sending initial ENQ")
			} else {
				fmt.Printf("  SYN-SYN-ENQ\n")
			}

			timeout := time.After(2500 * time.Millisecond)

		loop:
			for {
				select {
				case reply := <-pipe:
					if slices.Equal(reply, SYN_SYN_ACK) {
						fmt.Printf("  ACK\n")
						break enq
					}

				case <-timeout:
					fmt.Printf("  TIMEOUT\n")
					break loop
				}
			}
		}

		defer func() {
			println("exiting apparently")
		}()

		// ... TX/RX loop
		idle := time.After(5000 * time.Millisecond)

		for {
			select {
			case reply := <-pipe:
				fmt.Printf(">>> READ %v\n", string(reply))

			case <-idle:
				return fmt.Errorf("idle")

			case err := <-errors:
				return err
			}
		}

		return nil
	}
}
