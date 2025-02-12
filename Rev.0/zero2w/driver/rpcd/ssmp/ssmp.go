package ssmp

import (
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

		if packet, err := BER.Encode(rq); err != nil {
			return nil, err
		} else if encoded, err := bisync.Encode(nil, packet); err != nil {
			return nil, err
		} else {
			pipe := make(chan result)

			f := func() {
				value, err := s.stub.Get(encoded)

				pipe <- result{
					value: value,
					err:   err,
				}
			}

			s.queue <- f

			v := <-pipe

			return v.value, v.err
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
