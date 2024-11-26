package ssmp

import (
	"fmt"
	"sync/atomic"
	"time"

	"github.com/gosnmp/gosnmp"

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

// func (ssmp SSMP) Run() {
// 	rx := make(chan []byte, 16)
// 	tx := make(chan []byte)
// 	pipe := make(chan []byte)
// 	errors := make(chan error)
// 	handlers := map[uint32]request{}
//
// 	defer func() {
// 		close(pipe)
// 	}()
//
// 	defer func() {
// 		close(errors)
// 	}()
//
// 	// ... reply channel
// 	go func() {
// 		codec := bisync.NewBisync()
//
// 		for {
// 			select {
// 			case msg := <-rx:
// 				replies := received(msg, codec)
// 				for _, reply := range replies {
// 					rqid := reply.RequestID
//
// 					if h, ok := handlers[rqid]; !ok {
// 						warnf("reply %v (missing handler)", rqid)
// 					} else {
// 						select {
// 						case h.pipe <- reply:
// 						default:
// 							warnf("reply %v (pipe closed)", rqid)
// 						}
// 					}
//
// 					delete(handlers, rqid)
// 				}
// 			}
// 		}
// 	}()
//
// 	// ... request channel
// 	go func() {
// 		codec := bisync.NewBisync()
//
// 		for {
// 			select {
// 			case request := <-write:
// 				if err := send(request.packet, codec, tx); err != nil {
// 					warnf("request error (%v)", err)
// 				} else {
// 					handlers[request.packet.RequestID] = request
// 				}
// 			}
// 		}
// 	}()
//
// 	// ... listener
// 	for {
// 		if err := listen(ssmp.USB, tx, rx, pipe, errors); err != nil {
// 			errorf("%v", err)
// 		}
//
// 		time.Sleep(1 * time.Second)
// 	}
// }

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
