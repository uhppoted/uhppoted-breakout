package serial

import (
	"fmt"

	"github.com/pkg/term"

	"ssmp/encoding/bisync"
	"ssmp/log"
)

const TAG = "SERIAL"

type Serial struct {
	deviceId string
	codec    *bisync.Bisync

	requests <-chan []byte
	replies  chan<- []byte
}

func New(deviceId string, requests <-chan []byte, replies chan<- []byte) (*Serial, error) {
	serial := Serial{
		deviceId: deviceId,
		codec:    bisync.NewBisync(),
		requests: requests,
		replies:  replies,
	}

	return &serial, nil
}

func (s *Serial) Run() error {
	infof("run::start")

	eof := make(chan bool)

	if t, err := term.Open(s.deviceId, term.Speed(115200), term.RawMode); err != nil {
		return err
	} else if err := t.SetRaw(); err != nil {
		return err
	} else {
		infof("connected %v", s.deviceId)

		defer func() {
			warnf("closing %v", s.deviceId)
			t.Close()
		}()

		go func() {
			for {
				buffer := make([]byte, 256)

				if N, err := t.Read(buffer); err != nil {
					warnf("%v", err)
					break
				} else if N > 0 {
					debugf("received %v bytes", N)

					s.replies <- buffer[0:N]
				}
			}

			eof <- true
		}()

		for {
			select {
			case rq := <-s.requests:
				if rq == nil {
					return nil
				} else if N, err := t.Write(rq); err != nil {
					warnf("%v", err)
				} else if N < len(rq) {
					warnf("sent %v of %v bytes", N, len(rq))
				}

			case <-eof:
				return fmt.Errorf("EOF")
			}
		}
	}

	return nil
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
