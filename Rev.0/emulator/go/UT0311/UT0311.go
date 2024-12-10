package UT0311

import (
	"reflect"
	"sync"
	"time"

	"github.com/uhppoted/uhppote-core/messages"

	"emulator/config"
	"emulator/driver"
	"emulator/log"
)

type UT0311 struct {
	config config.Config
	driver driver.Driver
	udp    UDP
	tcp    TCP
	tls    TLS
}

func (ut0311 *UT0311) SetConfig(c config.Config) {
	ut0311.config = c
}

func NewUT0311(c config.Config, d driver.Driver) UT0311 {
	return UT0311{
		config: c,
		driver: d,
		udp:    UDP{},
		tcp:    TCP{},
		tls:    TLS{},
	}
}

func (ut0311 *UT0311) Run() {
	var wg sync.WaitGroup

	// ... start UDP listener
	wg.Add(1)

	go func() {
		for {
			if err := ut0311.udp.listen(ut0311.received); err != nil {
				warnf("%v", err)
			}

			// TODO: exponential backoff
			time.Sleep(5 * time.Second)
		}

		wg.Done()
	}()

	// ... start TCP listener
	wg.Add(1)

	go func() {
		for {
			if err := ut0311.tcp.listen(ut0311.received); err != nil {
				warnf("%v", err)
			}

			// TODO: exponential backoff
			time.Sleep(5 * time.Second)
		}
		wg.Done()
	}()

	// ... start TLS listener
	wg.Add(1)

	go func() {
		for {
			if err := ut0311.tls.listen(ut0311.received); err != nil {
				warnf("%v", err)
			}

			// TODO: exponential backoff
			time.Sleep(5 * time.Second)
		}
		wg.Done()
	}()

	// ... 'k, done

	wg.Wait()
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
