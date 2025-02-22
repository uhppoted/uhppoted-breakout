package ssmp

import (
	"fmt"
	"time"

	"ssmp/encoding/ASN.1"
	"ssmp/encoding/bisync"
	"ssmp/log"
	"ssmp/ssmp/stub"
)

type SSMP struct {
	queue chan func()
	stub  *stub.Stub
}

type result = struct {
	value any
	err   error
}

type callback struct {
	pipe chan []uint8
}

func (c callback) OnENQ() {
}

func (c callback) OnACK() {
}

func (c callback) OnMessage(header []uint8, content []uint8) {
	select {
	case c.pipe <- content:
	default:
	}
}

func NewSSMP() *SSMP {
	ssmp := SSMP{
		queue: make(chan func()),
		stub:  stub.NewStub(),
	}

	return &ssmp
}

func (s *SSMP) Run() error {
	infof("run::start")

loop:
	for {
		select {
		case f, ok := <-s.queue:
			if !ok {
				break loop
			} else {
				f()
			}
		}
	}

	infof("run::exit")
	return nil
}

func (s *SSMP) Stop() error {
	infof("run::stop")
	close(s.queue)

	return nil
}

func (s *SSMP) Get(oid string) (any, error) {
	debugf("get %v", oid)

	if o, err := BER.ParseOID(oid); err != nil {
		return nil, err
	} else {
		rq := BER.GetRequest{
			Version:    0,
			Community:  "public",
			RequestID:  12345,
			Error:      0,
			ErrorIndex: 0,
			OID:        o,
		}

		if packet, err := BER.EncodeGetRequest(rq); err != nil {
			return nil, err
		} else if encoded, err := bisync.Encode(nil, packet); err != nil {
			return nil, err
		} else {
			codec := bisync.NewBisync()
			pipe := make(chan result)
			h := callback{
				pipe: make(chan []uint8, 1),
			}

			f := func() {
				if reply, err := s.stub.Get(encoded); err != nil {
					pipe <- result{
						value: nil,
						err:   err,
					}
				} else if err := codec.Decode(reply, h); err != nil {
					pipe <- result{
						value: nil,
						err:   err,
					}
				} else {
					select {
					case <-time.After(100 * time.Millisecond):
						pipe <- result{
							value: nil,
							err:   fmt.Errorf("timeout decoding response"),
						}

					case decoded := <-h.pipe:
						if packet, err := BER.Decode(decoded); err != nil {
							pipe <- result{
								value: nil,
								err:   err,
							}
						} else if response, ok := packet.(BER.GetResponse); !ok {
							pipe <- result{
								value: nil,
								err:   fmt.Errorf("invalid reply type (%T)", packet),
							}
						} else if response.RequestID != rq.RequestID {
							pipe <- result{
								value: nil,
								err:   fmt.Errorf("invalid response ID (%v)", response.RequestID),
							}
						} else if response.Error != 0 {
							pipe <- result{
								value: nil,
								err:   fmt.Errorf("error code %v at index %v", response.Error, response.ErrorIndex),
							}
						} else {
							pipe <- result{
								value: response.Value,
								err:   nil,
							}
						}
					}
				}
			}

			s.queue <- f

			if v := <-pipe; v.err != nil {
				return nil, err
			} else {
				return v.value, nil
			}
		}
	}
}

func (s *SSMP) Set(oid string, value any) (any, error) {
	debugf("set %v %v", oid, value)

	pipe := make(chan result)

	f := func() {
		value, err := s.stub.Set(oid, value)

		pipe <- result{
			value: value,
			err:   err,
		}
	}

	s.queue <- f

	v := <-pipe

	return v.value, v.err
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
