package ssmp

import (
	"fmt"
	syslog "log"
	"os"
	"slices"
	"sync/atomic"
	"time"

	"github.com/gosnmp/gosnmp"
	"github.com/pkg/term"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/log"
)

type SSMP struct {
	USB string
}

type request struct {
	id     uint32
	packet []byte
}

var write = make(chan request)
var rqid = atomic.Uint32{}

func Get(oid []uint32) (any, error) {
	logger := gosnmp.NewLogger(syslog.New(os.Stdout, "", 0))

	pdu := gosnmp.SnmpPDU{
		Value: nil,
		Name:  ".1.3.6.655136.1.1",
		Type:  gosnmp.Null,
	}

	packet := gosnmp.SnmpPacket{
		Version:         gosnmp.Version1,
		ContextEngineID: "ssmp",
		ContextName:     "ssmp",
		Community:       "public",
		PDUType:         gosnmp.GetRequest,
		MsgID:           1,
		RequestID:       1,
		MsgMaxSize:      512,
		Error:           gosnmp.NoError,
		ErrorIndex:      0,
		NonRepeaters:    0,
		MaxRepetitions:  0,
		Variables:       []gosnmp.SnmpPDU{pdu},
		Logger:          logger,
	}

	if bytes, err := packet.MarshalMsg(); err != nil {
		return nil, err
	} else {
		id := rqid.Add(1)
		if encoded, err := encode(id, bytes); err != nil {
			return nil, err
		} else {
			write <- request{
				id:     id,
				packet: encoded,
			}
		}
	}

	return uint32(405419896), nil
}

func (ssmp SSMP) Run() {
	for {
		if err := ssmp.read(); err != nil {
			errorf("%v", err)
		}

		time.Sleep(1 * time.Second)
	}
}

func (ssmp SSMP) read() error {
	if t, err := term.Open(ssmp.USB, term.Speed(9600), term.RawMode); err != nil {
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

			timeout := time.After(1000 * time.Millisecond)

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
		idle := time.After(39000 * time.Millisecond)

		for {
			select {
			case reply := <-pipe:
				debugf("read %v %v", len(reply), reply)

			case request := <-write:
				debugf("write %v", request)
				if N, err := t.Write(request.packet); err != nil {
					warnf("write error (%v)", err)
				} else {
					debugf("write (%v bytes)", N)
				}

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
