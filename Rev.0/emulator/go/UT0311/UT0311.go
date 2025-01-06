package UT0311

import (
	"errors"
	"net"
	"reflect"
	"sync"
	"time"

	"github.com/uhppoted/uhppote-core/messages"

	"emulator/config"
	"emulator/driver/rpcd"
	"emulator/events"
	"emulator/log"
	"emulator/system"
)

type listener interface {
	listen(received func(any) (any, error)) error
	stop() error
	isClosing() bool
}

type UT0311 struct {
	config config.Config
	driver rpcd.RPC
	system system.System
	events events.Events

	udp *UDP
	tcp *TCP
	tls *TLS

	closing bool
}

func (ut0311 *UT0311) SetConfig(c config.Config) {
	ut0311.config = c
}

func NewUT0311(c config.Config) UT0311 {
	return UT0311{
		config: c,
		driver: rpcd.RPC{},
		system: system.System{},
		events: events.Events{},

		udp: makeUDP(),
		tcp: makeTCP(),
		tls: makeTLS(c.TLS.Certificate, c.TLS.CA),

		closing: false,
	}
}

func (ut0311 *UT0311) Run() {
	var wg sync.WaitGroup

	// ... start UDP listener
	wg.Add(1)
	go func() {
		for !ut0311.closing {
			if err := ut0311.listen(ut0311.udp); err != nil {
				warnf("%v", err)
			}

			// TODO: exponential backoff
			time.Sleep(2500 * time.Millisecond)
		}

		wg.Done()
	}()

	// ... start TCP listener
	wg.Add(1)
	go func() {
		for !ut0311.closing {
			if err := ut0311.listen(ut0311.tcp); err != nil {
				warnf("%v", err)
			}

			// TODO: exponential backoff
			time.Sleep(2500 * time.Millisecond)
		}
		wg.Done()
	}()

	// ... start TLS listener
	wg.Add(1)
	go func() {
		for !ut0311.closing {
			if err := ut0311.listen(ut0311.tls); err != nil {
				warnf("%v", err)
			}

			// TODO: exponential backoff
			time.Sleep(2500 * time.Millisecond)
		}
		wg.Done()
	}()

	// ... 'k, done

	wg.Wait()
}

func (ut0311 *UT0311) Stop() {
	infof("stopping")

	var wg sync.WaitGroup

	closed := make(chan struct{})
	timeout := time.Duration(10) * time.Second

	ut0311.closing = true

	wg.Add(1)
	go func() {
		infof("stopping UDP")
		if err := ut0311.udp.stop(); err != nil {
			warnf("%v", err)
		}

		wg.Done()
	}()

	wg.Add(1)
	go func() {
		infof("stopping TCP")
		if err := ut0311.tcp.stop(); err != nil {
			warnf("%v", err)
		}

		wg.Done()
	}()

	wg.Add(1)
	go func() {
		infof("stopping TLS")
		if err := ut0311.tls.stop(); err != nil {
			warnf("%v", err)
		}

		wg.Done()
	}()

	go func() {
		wg.Wait()
		closed <- struct{}{}
	}()

	select {
	case <-closed:
		infof("terminated")
	case <-time.After(timeout):
		warnf("shutdown timeout")
	}
}

func (ut0311 UT0311) listen(c listener) error {
	if err := c.listen(ut0311.received); err != nil {
		if errors.Is(err, net.ErrClosed) && c.isClosing() {
			return nil
		} else {
			return err
		}
	}

	return nil
}

func (ut0311 UT0311) received(request any) (any, error) {
	infof("UDP  request %T", request)

	switch rq := request.(type) {
	case *messages.GetDeviceRequest:
		return ut0311.getController(rq)

	case *messages.SetAddressRequest:
		return ut0311.setAddress(rq)

	case *messages.GetListenerRequest:
		return ut0311.getListener(rq)

	case *messages.SetListenerRequest:
		return ut0311.setListener(rq)

	case *messages.GetTimeRequest:
		return ut0311.getTime(rq)

	case *messages.SetTimeRequest:
		return ut0311.setTime(rq)

	case *messages.GetStatusRequest:
		return ut0311.getStatus(rq)

	case *messages.GetDoorControlStateRequest:
		return ut0311.getDoor(rq)

	case *messages.SetDoorControlStateRequest:
		return ut0311.setDoor(rq)

	case *messages.SetDoorPasscodesRequest:
		return ut0311.setDoorPasscodes(rq)

	default:
		warnf("unknown message type (%T)", request)
	}

	return nil, nil
}

func isnil(v any) bool {
	if v == nil {
		return true
	}

	switch reflect.TypeOf(v).Kind() {
	case reflect.Ptr,
		reflect.Map,
		reflect.Array,
		reflect.Chan,
		reflect.Slice:
		return reflect.ValueOf(v).IsNil()
	}

	return false
}

func debugf(format string, args ...any) {
	log.Debugf("UT0311", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("UT0311", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("UT0311", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("UT0311", format, args...)
}
