package ssmp

import (
	"fmt"
	"slices"
	"time"

	"github.com/pkg/term"
	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/log"
)

type SSMP struct {
	USB string
}

func Get(oid []uint32) (any, error) {
	return uint32(405419896), nil
}

func (ssmp SSMP) Run() {
	for {
		if err := read(ssmp.USB); err != nil {
			errorf("%v", err)
		}

		time.Sleep(1 * time.Second)
	}
}

func read(USB string) error {
	if t, err := term.Open(USB, term.Speed(115200), term.RawMode); err != nil {
		return err
	} else {
		infof("connected")

		// NTS: term.Close hangs indefinitely if a read is pending
		//      (seems happy enough to reopen the connection in any event)
		// defer func() {
		//  if err := t.Close(); err != nil {
		//      fmt.Printf("  *** ERROR %v\n", err)
		//  }
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
				debugf("SYN-SYN-ENQ")
			}

			timeout := time.After(30000 * time.Millisecond)

		loop:
			for {
				select {
				case reply := <-pipe:
					if slices.Equal(reply, SYN_SYN_ACK) {
						debugf("ACK")
						break enq
					}

				case <-timeout:
					warnf("TIMEOUT")
					break loop
				}
			}
		}

		// ... TX/RX loop
		idle := time.After(5000 * time.Millisecond)

		for {
			select {
			case reply := <-pipe:
				debugf("read %v", string(reply))

			case <-idle:
				return fmt.Errorf("idle")

			case err := <-errors:
				return err
			}
		}

		return nil
	}
}

func debugf(format string, args ...any) {
	log.Debugf("SSMP", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("SSMP", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("SSMP", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("SSMP", format, args...)
}
