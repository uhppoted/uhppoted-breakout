package ssmp

import (
	"fmt"
	"sync/atomic"
	"time"

	"github.com/gosnmp/gosnmp"
	"github.com/pkg/term"

	"emulator/MIB/types"
	"emulator/encoding/BER"
	ssmp "emulator/encoding/SSMP"
	"emulator/encoding/bisync"
	"emulator/log"
)

type SSMP struct {
	USB string
}

type request struct {
	packet ssmp.GetPacket
	pipe   chan gosnmp.SnmpPacket
}

var write = make(chan request)
var rqid = atomic.Uint32{}

func Get(oid types.OID) (any, error) {
	if rqId, err := nextID(); err != nil {
		return nil, err
	} else {
		packet := ssmp.GetPacket{
			Community: "public",
			RequestID: rqId,
			OID:       fmt.Sprintf("%v", oid),
		}

		rq := request{
			packet: packet,
			pipe:   make(chan gosnmp.SnmpPacket),
		}

		timeout := time.After(500 * time.Millisecond)

		write <- rq
		select {
		case reply := <-rq.pipe:
			debugf("reply to SSMP GET request (%v)", reply)

			if value, err := get(reply, oid); err != nil {
				return 0, fmt.Errorf("invalid reply to SSMP GET %v request", oid)
			} else if v, ok := value.(int); ok {
				return uint32(v), nil
			} else if v, ok := value.([]uint8); ok {
				return []uint8(v), nil
			} else {
				return 0, fmt.Errorf("invalid value (%T) in reply to SSMP GET %v request", v, oid)
			}

		case <-timeout:
			return 0, fmt.Errorf("no reply to SSMP GET %v request", oid)
		}

		return 0, fmt.Errorf("SSMP GET %v failed", oid)
	}
}

func (ssmp SSMP) Run() {
	rx := make(chan []byte, 16)
	tx := make(chan []byte)
	pipe := make(chan []byte)
	errors := make(chan error)
	handlers := map[uint32]request{}

	defer func() {
		close(pipe)
	}()

	defer func() {
		close(errors)
	}()

	// ... reply channel
	go func() {
		codec := bisync.NewBisync()

		for {
			select {
			case msg := <-rx:
				replies := received(msg, codec)
				for _, reply := range replies {
					rqid := reply.RequestID

					if h, ok := handlers[rqid]; !ok {
						warnf("reply %v (missing handler)", rqid)
					} else {
						select {
						case h.pipe <- reply:
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
		codec := bisync.NewBisync()

		for {
			select {
			case request := <-write:
				if err := send(request.packet, codec, tx); err != nil {
					warnf("request error (%v)", err)
				} else {
					handlers[request.packet.RequestID] = request
				}
			}
		}
	}()

	// ... listener
	for {
		if err := listen(ssmp.USB, tx, rx, pipe, errors); err != nil {
			errorf("%v", err)
		}

		time.Sleep(1 * time.Second)
	}
}

func send(packet ssmp.GetPacket, codec *bisync.Bisync, tx chan []byte) error {
	if bytes, err := packet.Encode(); err != nil {
		return err
	} else {
		if encoded, err := codec.Encode(nil, bytes); err != nil {
			return err
		} else {
			select {
			case tx <- encoded:
				return nil

			default:
				return fmt.Errorf("TX queue blocked")
			}
		}
	}
}

type handler struct {
	onENQ     func()
	onACK     func()
	onMessage func(header []uint8, data []uint8)
}

func (h handler) OnENQ() {
	if h.onENQ != nil {
		h.onENQ()
	}
}

func (h handler) OnACK() {
	if h.onACK != nil {
		h.onACK()
	}
}

func (h handler) OnMessage(header []uint8, content []uint8) {
	if h.onMessage != nil {
		h.onMessage(header, content)
	}
}

func received(msg []byte, codec *bisync.Bisync) []gosnmp.SnmpPacket {
	println(">>>>>>>>> received ", len(msg))
	var packets []gosnmp.SnmpPacket

	h := handler{
		onENQ: func() {
			debugf("ENQ")
		},

		onACK: func() {
			debugf("ACK")
		},

		onMessage: func(header []uint8, content []uint8) {
			if packet, err := BER.Decode(content); err != nil {
				warnf("%v", err)
			} else if packet == nil {
				warnf("invalid packet (%v)", packet)
			} else {
				packets = append(packets, *packet)
				println(">>>>>>>>> received/gotcha", len(packets))
			}
		},
	}

	if err := codec.Decode(msg, h); err != nil {
		warnf("%v", err)
	}

	return packets
}

func get(packet gosnmp.SnmpPacket, oid types.OID) (any, error) {
	name := fmt.Sprintf("%v", oid)

	for _, pdu := range packet.Variables {
		if pdu.Name == name {
			return pdu.Value, nil
		}
	}

	return nil, fmt.Errorf("OID %v not found", oid)
}

func listen(USB string, tx chan []byte, rx chan []byte, pipe chan []byte, errors chan error) error {
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

		time.Sleep(100 * time.Millisecond)

		// ... read
		closed := false

		defer func() {
			closed = true
		}()

		go func() {
			buffer := make([]byte, 256)

			for !closed {
				if N, err := t.Read(buffer); err != nil {
					errors <- err
					return
				} else if N > 0 {
					pipe <- buffer[0:N]
				}
			}
		}()

		// ... send SYN-SYN-ENQ at 1s intervals until acknowledged
		codec := bisync.NewBisync()
		acknowledged := false

		h := handler{
			onACK: func() {
				acknowledged = true
			},
		}

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
					if err := codec.Decode(reply, h); err != nil {
						return err
					} else if acknowledged {
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
		idle := time.After(15000 * time.Millisecond)
		ping := time.Tick(2500 * time.Millisecond)

		for {
			select {
			case msg := <-pipe:
				if len(msg) > 2 && msg[0] == '>' && msg[1] == '>' {
					debugf("read  (%v bytes) %v", len(msg), string(msg))
				} else if len(msg) > 50 {
					debugf("read  (%v bytes) [%v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v %v ...]",
						len(msg),
						msg[0], msg[1], msg[2], msg[3], msg[4], msg[5], msg[6], msg[7], msg[8], msg[9],
						msg[10], msg[11], msg[12], msg[13], msg[14], msg[15], msg[16], msg[17], msg[18], msg[19])
				} else {
					debugf("read  (%v bytes) %v", len(msg), msg)
				}

				select {
				case rx <- msg:
					// debugf("read  ok")
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

			case <-ping:
				cmd := bisync.Enq
				if N, err := t.Write(cmd); err != nil {
					warnf("ping error (%v)", err)
				} else if N != len(cmd) {
					warnf("ping error (sent %v of %v bytes)", N, len(cmd))
				} else {
					debugf("SYN-SYN-ENQ")
				}

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
