package ssmp

import (
	"fmt"
	"sync"
	"sync/atomic"
	"time"

	"ssmp/encoding/ASN.1"
	"ssmp/encoding/bisync"
	"ssmp/log"
	"ssmp/ssmp/serial"
	"ssmp/ssmp/stub"
)

const DELAY = 10 * time.Second
const RESET = 15 * time.Second

type SSMP struct {
	codec *bisync.Bisync

	queue    chan func()
	requests chan []byte
	rx       chan []byte
	wg       sync.WaitGroup
	pending  pending[BER.GetResponse]

	driver interface {
		Run() error
	}
}

type callback struct {
	error    func(err error)
	dispatch func(packet any)
}

func (c callback) OnENQ() {
}

func (c callback) OnACK() {
}

func (c callback) OnMessage(header []uint8, content []uint8) {
	if packet, err := BER.Decode(content); err != nil {
		c.error(err)
	} else {
		c.dispatch(packet)
	}
}

var ID atomic.Uint32

func NewSSMP(deviceId string) (*SSMP, error) {
	requests := make(chan []byte, 1)
	rx := make(chan []byte, 1)

	ssmp := SSMP{
		codec:    bisync.NewBisync(),
		queue:    make(chan func()),
		requests: requests,
		rx:       rx,
		wg:       sync.WaitGroup{},
		pending: pending[BER.GetResponse]{
			m: map[uint32]e[BER.GetResponse]{},
		},
	}

	if deviceId == "stub" {
		ssmp.driver = stub.New(requests, rx)
	} else if d, err := serial.New(deviceId, requests, rx); err != nil {
		return nil, err
	} else {
		ssmp.driver = d
	}

	return &ssmp, nil
}

func (s *SSMP) Run() error {
	infof("run::start")

	// ... RX queue
	s.wg.Add(1)
	go func() {
		defer s.wg.Done()
		s.receive()
	}()

	// ... TX queue
	s.wg.Add(1)
	go func() {
		defer s.wg.Done()
		s.dequeue()
	}()

	// ... housekeeping
	go func() {
		tick := time.Tick(5 * time.Second)
		for range tick {
			s.pending.sweep()
		}
	}()

	// ... interface
	s.wg.Add(1)
	go func() {
		defer s.wg.Done()

		delay := 500 * time.Millisecond

		for {
			reset := time.AfterFunc(RESET, func() {
				delay = 500 * time.Millisecond
			})

			if err := s.driver.Run(); err == nil {
				break
			} else {
				warnf("%v", err)
				infof("disconected .. reconnecting in %v", delay)

				reset.Stop()
				time.Sleep(delay)

				delay = 2 * delay
				if delay > DELAY {
					delay = DELAY
				}
			}
		}
	}()

	infof("run::exit")

	return nil
}

func (s *SSMP) receive() {
	h := callback{
		error: func(err error) {
			errorf("%v", err)
		},

		dispatch: func(packet any) {
			s.dispatch(packet)
		},
	}

	for packet := range s.rx {
		if err := s.codec.Decode(packet, h); err != nil {
			warnf("%v", err)
		}
	}

	infof("rx::exit")
}

func (s *SSMP) dequeue() {
	for f := range s.queue {
		f()
	}

	infof("tx::exit")
}

func (s *SSMP) dispatch(packet any) {
	switch v := packet.(type) {
	case BER.GetResponse:
		if h := s.pending.get(v.RequestID); h != nil {
			h(v)
		} else {
			warnf("no handler for GET response %v", v.RequestID)
		}

	default:
		errorf("unknown packet type (%T)", packet)
	}

}

func (s *SSMP) Stop() error {
	infof("run::stop")

	c := make(chan struct{})

	go func() {
		close(s.requests)
		close(s.rx)
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
			Version:   0,
			Community: "public",
			RequestID: ID.Add(1),
			OID:       o,
		}

		if packet, err := BER.EncodeGetRequest(rq); err != nil {
			return nil, err
		} else if encoded, err := bisync.Encode(nil, packet); err != nil {
			return nil, err
		} else {
			pipe := make(chan BER.GetResponse, 1)
			done := make(chan struct{})

			defer close(pipe)
			defer close(done)
			defer s.pending.delete(rq.RequestID)

			s.queue <- func() {
				s.pending.put(rq.RequestID, func(packet BER.GetResponse) {
					pipe <- packet
				})

				s.requests <- encoded

				<-done
			}

			select {
			case <-time.After(2500 * time.Millisecond):
				return nil, fmt.Errorf("timeout")

			case response := <-pipe:
				if response.Error != 0 {
					return nil, fmt.Errorf("error code %v at index %v", response.Error, response.ErrorIndex)
				} else {
					return response.Value, nil
				}
			}
		}
	}
}

func (s *SSMP) Set(oid string, value any) (any, error) {
	debugf("set %v %v", oid, value)

	if o, err := BER.ParseOID(oid); err != nil {
		return nil, err
	} else {
		rq := BER.SetRequest{
			Version:   0,
			Community: "public",
			RequestID: ID.Add(1),
			OID:       o,
			Value:     value,
		}

		if packet, err := BER.EncodeSetRequest(rq); err != nil {
			return nil, err
		} else if encoded, err := bisync.Encode(nil, packet); err != nil {
			return nil, err
		} else {
			pipe := make(chan BER.GetResponse, 1)
			done := make(chan struct{})

			defer close(pipe)
			defer close(done)
			defer s.pending.delete(rq.RequestID)

			s.queue <- func() {
				s.pending.put(rq.RequestID, func(packet BER.GetResponse) {
					pipe <- packet
				})

				s.requests <- encoded

				<-done
			}

			select {
			case <-time.After(2500 * time.Millisecond):
				return nil, fmt.Errorf("timeout")

			case response := <-pipe:
				if response.Error != 0 {
					return nil, fmt.Errorf("error code %v at index %v", response.Error, response.ErrorIndex)
				} else {
					return response.Value, nil
				}
			}
		}
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
