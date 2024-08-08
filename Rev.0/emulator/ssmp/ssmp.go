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
	packet []byte
	pipe   chan []byte
}

var write = make(chan request)
var rqid = atomic.Uint32{}

func Get(oid []uint32) (any, error) {
	logger := gosnmp.NewLogger(syslog.New(os.Stdout, "", 0))

	pdu := gosnmp.SnmpPDU{
		Value: uint32(405419896),
		Name:  ".1.3.6.655136.1.1",
		Type:  gosnmp.Uinteger32,
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
		rq := request{
			packet: bytes,
			pipe:   make(chan []byte),
		}

		timeout := time.After(500 * time.Millisecond)

		write <- rq
		select {
		case reply := <-rq.pipe:
			debugf("reply to SSMP GET request (%v)", reply)

			if value, err := get(reply, ".1.3.6.655136.1.1"); err != nil {
				return 0, fmt.Errorf("invalid reply to SSMP GET %v request", ".1.3.6.655136.1.1")
			} else if v, ok := value.(uint32); !ok {
				return 0, fmt.Errorf("invalid valid in reply to SSMP GET %v request", ".1.3.6.655136.1.1")
			} else {
				return v, nil
			}

		case <-timeout:
			return 0, fmt.Errorf("no reply to SSMP GET %v request", ".1.3.6.655136.1.1")
		}
	}

	return 0, fmt.Errorf("SSMP GET %v failed", ".1.3.6.655136.1.1")
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
	handlers := map[uint32]request{}

	if t, err := term.Open(ssmp.USB, term.Speed(115200), term.RawMode); err != nil {
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
			case msg := <-pipe:
				debugf("read %v %v", len(msg), msg)
				if id, reply, err := decode(msg); err != nil {
					warnf("read error (%v)", err)
				} else {
					debugf("read %v %v", id, reply)

					if h, ok := handlers[id]; ok {
						h.pipe <- reply
					} else {
						warnf("read error (%v)", fmt.Errorf("missing handler for message %v", id))
					}

					delete(handlers, id)
				}

			case request := <-write:
				debugf("write %v", request)
				id := uint32(1) // rqid.Add(1)
				if encoded, err := encode(id, request.packet); err != nil {
					warnf("write error (%v)", err)
				} else if N, err := t.Write(encoded); err != nil {
					warnf("write error (%v)", err)
				} else if N != len(encoded) {
					warnf("write error (%v)", fmt.Errorf("sent %v bytes of %v)", N, len(encoded)))
				} else {
					debugf("write (%v bytes)", N)

					handlers[id] = request
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

func get(msg []uint8, OID string) (any, error) {
	snmp := gosnmp.GoSNMP{}

	if packet, err := snmp.SnmpDecodePacket(msg); err != nil {
		return nil, err
	} else {
		for _, pdu := range packet.Variables {
			if pdu.Name == OID {
				return pdu.Value, nil
			}
		}
	}

	return nil, fmt.Errorf("OID %v not found", OID)
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
