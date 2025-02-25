package ssmp

import (
	"fmt"
	"sync"
	"sync/atomic"
	"time"

	"ssmp/encoding/ASN.1"
	"ssmp/encoding/bisync"
	"ssmp/log"
	"ssmp/ssmp/stub"
)

type SSMP struct {
	queue    chan func()
	requests chan []byte
	rx       chan []byte
	wg       sync.WaitGroup

	pending pending
	stub    *stub.Stub
}

type pending struct {
	m     map[uint32]func(packet BER.GetResponse)
	guard sync.RWMutex
}

func (p *pending) put(id uint32, f func(packet BER.GetResponse)) {
	p.guard.Lock()
	defer p.guard.Unlock()

	p.m[id] = f
}

func (p *pending) delete(id uint32) {
	p.guard.Lock()
	defer p.guard.Unlock()

	delete(p.m, id)
}

func (p *pending) get(id uint32) func(packet BER.GetResponse) {
	p.guard.RLock()
	defer p.guard.RUnlock()

	if f, ok := p.m[id]; ok {
		return f
	}

	return nil
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

func NewSSMP() *SSMP {
	requests := make(chan []byte, 1)
	rx := make(chan []byte, 1)
	ssmp := SSMP{
		queue:    make(chan func()),
		requests: requests,
		rx:       rx,
		wg:       sync.WaitGroup{},
		pending: pending{
			m: map[uint32]func(packet BER.GetResponse){},
		},
		stub: stub.NewStub(requests, rx),
	}

	return &ssmp
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

	// ... stub
	s.wg.Add(1)
	go func() {
		defer s.wg.Done()
		s.stub.Run()
	}()

	infof("run::exit")

	return nil
}

func (s *SSMP) receive() {
	codec := bisync.NewBisync()

	h := callback{
		error: func(err error) {
			errorf("%v", err)
		},

		dispatch: func(packet any) {
			s.dispatch(packet)
		},
	}

	for packet := range s.rx {
		if err := codec.Decode(packet, h); err != nil {
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
			Version:    0,
			Community:  "public",
			RequestID:  ID.Add(1),
			Error:      0,
			ErrorIndex: 0,
			OID:        o,
		}

		if packet, err := BER.EncodeGetRequest(rq); err != nil {
			return nil, err
		} else if encoded, err := bisync.Encode(nil, packet); err != nil {
			return nil, err
		} else {
			pipe := make(chan BER.GetResponse, 1)

			defer close(pipe)
			defer s.pending.delete(rq.RequestID)

			s.queue <- func() {
				s.pending.put(rq.RequestID, func(packet BER.GetResponse) {
					pipe <- packet
				})

				s.requests <- encoded
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

	// pipe := make(chan result)
	//
	// f := func() {
	// 	value, err := s.stub.Set(oid, value)
	//
	// 	pipe <- result{
	// 		value: value,
	// 		err:   err,
	// 	}
	// }
	//
	// s.queue <- f
	//
	// v := <-pipe
	//
	// return v.value, v.err

	return nil, fmt.Errorf("NOT IMPLEMENTED")
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
