package ssmp

import (
	"fmt"
	"sync"
	"time"

	"ssmp/encoding/ASN.1"
	"ssmp/encoding/bisync"
	"ssmp/log"
	"ssmp/ssmp/stub"
)

type SSMP struct {
	queue    chan func()
	requests chan []byte
	replies  chan []byte
	wg       sync.WaitGroup
	stub     *stub.Stub
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
	requests := make(chan []byte, 1)
	replies := make(chan []byte, 1)
	ssmp := SSMP{
		queue:    make(chan func()),
		requests: requests,
		replies:  replies,
		wg:       sync.WaitGroup{},
		stub:     stub.NewStub(requests, replies),
	}

	return &ssmp
}

func (s *SSMP) Run() error {
	infof("run::start")

	// ... stub
	s.wg.Add(1)
	go func() {
		defer s.wg.Done()
		s.stub.Run()
	}()

	// ... dequeue
	s.wg.Add(1)
	go func() {
		defer s.wg.Done()
		s.dequeue()
	}()

	infof("run::exit")

	return nil
}

func (s *SSMP) dequeue() {
loop:
	for {
		select {
		case f, ok := <-s.queue:
			if ok {
				f()
			} else {
				break loop
			}
		}
	}
}

func (s *SSMP) Stop() error {
	infof("run::stop")

	c := make(chan struct{})

	go func() {
		close(s.requests)
		close(s.replies)
		close(s.queue)

		s.wg.Wait()

		c <- struct{}{}
	}()

	select {
	case <-c:
		return nil

	case <-time.After(2500 * time.Millisecond):
		return nil
	}
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
				s.requests <- encoded

				select {
				case reply := <-s.replies:
					if err := codec.Decode(reply, h); err != nil {
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

				case <-time.After(2500 * time.Millisecond):
					pipe <- result{
						value: nil,
						err:   fmt.Errorf("timeout"),
					}
				}
				// }
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
