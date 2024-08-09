package ssmp

import (
	"encoding/binary"
	"fmt"
	syslog "log"
	"os"
	"slices"
	"sync/atomic"
	"time"

	"github.com/gosnmp/gosnmp"
	"github.com/pkg/term"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/encoding/BER"
	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/encoding/bisync"
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
		Value: gosnmp.Null,
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

	if bytes, err := BER.Encode(packet); err != nil {
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
				return 0, fmt.Errorf("invalid value in reply to SSMP GET %v request", ".1.3.6.655136.1.1")
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
	rx := make(chan []byte)
	tx := make(chan []byte)
	handlers := map[uint32]request{}

	// ... reply channel
	go func() {
		codec := bisync.Bisync{
			SOH: false,
			STX: false,
			DLE: false,
		}

		for {
			select {
			case msg := <-rx:
				debugf("reply %v", msg)
				replies, err := codec.Decode(msg)
				if err != nil {
					warnf("reply error (%v)", err)
				}

				for _, reply := range replies {
					debugf("reply %v %v", rqid, reply)

					rqid := uint32(0)
					if len(reply.Header) >= 4 {
						rqid = binary.BigEndian.Uint32(reply.Header)
					}

					if h, ok := handlers[rqid]; !ok {
						warnf("reply %v (missing handler)", rqid)
					} else {
						select {
						case h.pipe <- reply.Packet:
							debugf("reply %v ok", rqid)
						default:
							warnf("reply %v (pipe closed)", rqid)
						}
					}

					delete(handlers, rqid)
				}
			}
		}
	}()

	// ... request channel
	go func() {
		codec := bisync.Bisync{}

		for {
			select {
			case request := <-write:
				debugf("request %v", request)

				msg := bisync.Message{
					Header: []byte{0x00, 0x00, 0x00, 0x01}, // FIXME use request ID from packet
					Packet: request.packet,
				}

				if encoded, err := codec.Encode(msg); err != nil {
					warnf("request error (%v)", err)
				} else {
					select {
					case tx <- encoded:
						handlers[1] = request // FIXME use request ID from packet
					default:
						warnf("TX queue blocked")
					}
				}
			}
		}
	}()

	// ... listener
	for {
		if err := ssmp.listen(tx, rx); err != nil {
			errorf("%v", err)
		}

		time.Sleep(1 * time.Second)
	}
}

func (ssmp SSMP) listen(tx chan []byte, rx chan []byte) error {
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
			cmd := bisync.Enq
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
					if slices.Equal(reply, bisync.Ack) {
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
				if len(msg) > 20 {
					debugf("read  (%v bytes) [%v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v ...]",
						len(msg),
						msg[0], msg[1], msg[2], msg[3], msg[4], msg[5], msg[6], msg[7], msg[8], msg[9],
						msg[10], msg[11], msg[12], msg[13], msg[14], msg[15], msg[16], msg[17], msg[18], msg[19])
				} else {
					debugf("read  (%v bytes) %v", len(msg), msg)
				}

				select {
				case rx <- msg:
					debugf("read  ok")
				default:
					warnf("RX queue blocked")
				}

			case msg := <-tx:
				if N, err := t.Write(msg); err != nil {
					warnf("write error (%v)", err)
				} else if N != len(msg) {
					warnf("write error (%v)", fmt.Errorf("sent %v bytes of %v)", N, len(msg)))
				} else if len(msg) > 20 {
					debugf("write (%v bytes) [%v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v ...]",
						N,
						msg[0], msg[1], msg[2], msg[3], msg[4], msg[5], msg[6], msg[7], msg[8], msg[9],
						msg[10], msg[11], msg[12], msg[13], msg[14], msg[15], msg[16], msg[17], msg[18], msg[19])
				} else {
					debugf("write (%v bytes) %v", N, msg)
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
	if packet, err := BER.Decode(msg); err != nil {
		return nil, err
	} else if packet == nil {
		return nil, fmt.Errorf("invalid SSMP packet (%v)", packet)
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
