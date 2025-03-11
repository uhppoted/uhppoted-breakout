package stub

import (
	"fmt"
	"time"

	"ssmp/encoding/ASN.1"
	"ssmp/encoding/bisync"
	"ssmp/log"
)

const TAG = "STUB"

type Stub struct {
	requests <-chan []byte
	replies  chan<- []byte

	codec *bisync.Bisync
}

func New(requests <-chan []byte, replies chan<- []byte) *Stub {
	stub := Stub{
		codec:    bisync.NewBisync(),
		requests: requests,
		replies:  replies,
	}

	return &stub
}

func (s *Stub) Run() error {
	infof("run::start %v", s.requests)

	h := handler{
		onENQ: func() {
			infof("ENQ")
		},

		onACK: func() {
			infof("ACK")
		},

		onMessage: func(header []uint8, content []uint8) {
			if packet, err := BER.Decode(content); err != nil {
				warnf("%v", err)
			} else if packet == nil {
				warnf("invalid packet (%v)", packet)
			} else if rq, ok := packet.(BER.GetRequest); ok {
				oid := fmt.Sprintf("%v", rq.OID)

				if v, err := s.get(oid); err != nil {
					warnf("error fulfilling GET request (%v)", err)
				} else {
					response := BER.GetResponse{
						Version:    0,
						Community:  "public",
						RequestID:  rq.RequestID,
						Error:      0,
						ErrorIndex: 0,
						OID:        rq.OID,
						Value:      v,
					}

					if packet, err := BER.EncodeGetResponse(response); err != nil {
						warnf("%v", err)
					} else if reply, err := bisync.Encode(nil, packet); err != nil {
						warnf("%v", err)
					} else {
						s.replies <- reply
					}
				}
			} else if rq, ok := packet.(BER.SetRequest); ok {
				oid := fmt.Sprintf("%v", rq.OID)
				value := rq.Value

				if v, err := s.set(oid, value); err != nil {
					warnf("error fulfilling SET request (%v)", err)
				} else {
					response := BER.GetResponse{
						Version:    0,
						Community:  "public",
						RequestID:  rq.RequestID,
						Error:      0,
						ErrorIndex: 0,
						OID:        rq.OID,
						Value:      v,
					}

					if packet, err := BER.EncodeGetResponse(response); err != nil {
						warnf("%v", err)
					} else if reply, err := bisync.Encode(nil, packet); err != nil {
						warnf("%v", err)
					} else {
						s.replies <- reply
					}
				}
			} else {
				warnf("unhandled packet type (%T)", packet)
			}
		},
	}

	for bytes := range s.requests {
		if err := s.codec.Decode(bytes, h); err != nil {
			warnf("error %v", err)
		}
	}

	infof("run::exit")

	return nil
}

func (s Stub) get(oid string) (any, error) {
	debugf("get %v", oid)

	// ... controller ID
	if oid == "0.1.3.6.1.4.1.65536.2.1" {
		return uint32(405419896), nil
	}

	// ... controller version
	if oid == "0.1.3.6.1.4.1.65536.2.2" {
		return uint16(0x1234), nil
	}

	// ... controller release date
	if oid == "0.1.3.6.1.4.1.65536.2.3" {
		return "2024-01-15", nil
	}

	// ... controller date/time
	if oid == "0.1.3.6.1.4.1.65536.2.8" {
		return time.Now().Format("2006-01-02 15:04:05"), nil
	}

	// ... controller error
	if oid == ".1.3.6.1.4.1.65536.2.9" {
		return uint8(0x12), nil
	}

	// ... controller special info
	if oid == ".1.3.6.1.4.1.65536.2.10" {
		return uint8(0x23), nil
	}

	// ... door 1 mode
	if oid == ".1.3.6.1.4.1.65536.3.1.3.1" {
		return uint8(1), nil
	}

	// ... door 1 delay
	if oid == ".1.3.6.1.4.1.65536.3.1.3.2" {
		return uint8(5), nil
	}

	// ... door 1 lock
	if oid == ".1.3.6.1.4.1.65536.3.1.4" {
		return true, nil
	}

	// ... door 1 open
	if oid == ".1.3.6.1.4.1.65536.3.1.5" {
		return true, nil
	}

	// ... door 1 button
	if oid == ".1.3.6.1.4.1.65536.3.1.6" {
		return true, nil
	}

	// ... door 2 mode
	if oid == ".1.3.6.1.4.1.65536.3.2.1" {
		return uint8(1), nil
	}

	// ... door 2 delay
	if oid == ".1.3.6.1.4.1.65536.3.2.2" {
		return uint8(5), nil
	}

	// ... door 2 lock
	if oid == ".1.3.6.1.4.1.65536.3.2.4" {
		return true, nil
	}

	// ... door 2 open
	if oid == ".1.3.6.1.4.1.65536.3.2.5" {
		return true, nil
	}

	// ... door 2 button
	if oid == ".1.3.6.1.4.1.65536.3.2.6" {
		return false, nil
	}

	// ... door 3 mode
	if oid == ".1.3.6.1.4.1.65536.3.3.1" {
		return uint8(3), nil
	}

	// ... door 3 delay
	if oid == ".1.3.6.1.4.1.65536.3.3.2" {
		return uint8(17), nil
	}

	// ... door 3 lock
	if oid == ".1.3.6.1.4.1.65536.3.3.4" {
		return true, nil
	}

	// ... door 3 open
	if oid == ".1.3.6.1.4.1.65536.3.3.5" {
		return false, nil
	}

	// ... door 3 button
	if oid == ".1.3.6.1.4.1.65536.3.3.6" {
		return true, nil
	}

	// ... door 4 mode
	if oid == ".1.3.6.1.4.1.65536.3.4.1" {
		return uint8(1), nil
	}

	// ... door 4 delay
	if oid == ".1.3.6.1.4.1.65536.3.4.2" {
		return uint8(5), nil
	}

	// ... door 4 lock
	if oid == ".1.3.6.1.4.1.65536.3.4.4" {
		return false, nil
	}

	// ... door 4 open
	if oid == ".1.3.6.1.4.1.65536.3.4.5" {
		return true, nil
	}

	// ... door 4 button
	if oid == ".1.3.6.1.4.1.65536.3.4.6" {
		return false, nil
	}

	// ... tamper detect
	if oid == ".1.3.6.1.4.1.65536.4.1" {
		return true, nil
	}

	// ... fire alarm
	if oid == ".1.3.6.1.4.1.65536.4.2" {
		return false, nil
	}

	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (s Stub) set(oid string, value any) (any, error) {
	debugf("set %v %v", oid, value)

	// ... controller date/time
	if oid == "0.1.3.6.1.4.1.65536.2.8" {
		return time.Now().Format("2006-01-02 15:04:05"), nil
	}

	// ... door 1 mode
	if oid == ".1.3.6.1.4.1.65536.3.1.1" {
		return uint8(1), nil
	}

	// ... door 1 delay
	if oid == ".1.3.6.1.4.1.65536.3.1.2" {
		return uint8(5), nil
	}

	// ... door 1 passcodes
	if oid == ".1.3.6.1.4.1.65536.3.1.3" {
		return []uint32{}, nil
	}

	// ... door 1 lock
	if oid == ".1.3.6.1.4.1.65536.3.1.4" {
		if b, ok := value.(bool); ok {
			return any(b), nil
		}
	}

	// ... door 2 mode
	if oid == ".1.3.6.1.4.1.65536.3.2.1" {
		return uint8(1), nil
	}

	// ... door 2 delay
	if oid == ".1.3.6.1.4.1.65536.3.2.2" {
		return uint8(5), nil
	}

	// ... door 2 passcodes
	if oid == ".1.3.6.1.4.1.65536.3.2.3" {
		return []uint32{}, nil
	}

	// ... door 2 lock
	if oid == ".1.3.6.1.4.1.65536.3.2.4" {
		if b, ok := value.(bool); ok {
			return any(b), nil
		}
	}

	// ... door 3 mode
	if oid == ".1.3.6.1.4.1.65536.3.3.1" {
		return uint8(3), nil
	}

	// ... door 3 delay
	if oid == ".1.3.6.1.4.1.65536.3.3.2" {
		return uint8(17), nil
	}

	// ... door 3 passcodes
	if oid == ".1.3.6.1.4.1.65536.3.3.3" {
		return []uint32{12345, 0, 999999, 54321}, nil
	}

	// ... door 3 lock
	if oid == ".1.3.6.1.4.1.65536.3.3.4" {
		if b, ok := value.(bool); ok {
			return any(b), nil
		}
	}

	// ... door 4 mode
	if oid == ".1.3.6.1.4.1.65536.3.4.1" {
		return uint8(1), nil
	}

	// ... door 4 delay
	if oid == ".1.3.6.1.4.1.65536.3.4.2" {
		return uint8(5), nil
	}

	// ... door 4 passcodes
	if oid == ".1.3.6.1.4.1.65536.3.4.3" {
		return []uint32{}, nil
	}

	// ... door 4 lock
	if oid == ".1.3.6.1.4.1.65536.3.4.4" {
		if b, ok := value.(bool); ok {
			return any(b), nil
		}
	}

	return nil, fmt.Errorf("unknown OID %v", oid)
}

func debugf(format string, args ...any) {
	log.Debugf(TAG, format, args...)
}

func infof(format string, args ...any) {
	log.Infof(TAG, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(TAG, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(TAG, format, args...)
}
