package UT0311

import (
	"errors"
	"fmt"
	"net"
	"reflect"
	"sync"
	"time"

	"golang.org/x/time/rate"

	"github.com/uhppoted/uhppote-core/messages"

	"emulator/cards"
	"emulator/config"
	"emulator/driver/rpcd"
	"emulator/events"
	"emulator/log"
	"emulator/system"
)

const BACKOFF = 500 * time.Millisecond
const MAX_BACKOFF = 60 * time.Second
const RATE_LIMIT = 5.0   // requests/second
const BURST_LIMIT = 10.0 // burst requests

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
	cards  cards.Cards

	udp       *UDP
	tcp       *TCP
	tls       *TLS
	rateLimit *rate.Limiter

	closing bool
}

func (ut0311 *UT0311) SetConfig(c config.Config) {
	ut0311.config = c
}

func NewUT0311(c config.Config) UT0311 {
	cm := NewConnectionManager(MAX_CONNECTIONS)
	return UT0311{
		config: c,
		driver: rpcd.RPC{},
		system: system.System{},
		events: events.Events{},
		cards:  cards.Cards{},

		udp:       makeUDP(cm),
		tcp:       makeTCP(cm),
		tls:       makeTLS(c.TLS.Certificate, c.TLS.CA, cm),
		rateLimit: rate.NewLimiter(RATE_LIMIT, BURST_LIMIT),

		closing: false,
	}
}

func (ut0311 *UT0311) Run() {
	var wg sync.WaitGroup

	// ... start UDP listener
	wg.Add(1)
	go func() {
		ut0311.listen("UDP", ut0311.udp)
		wg.Done()
	}()

	// ... start TCP listener
	wg.Add(1)
	go func() {
		ut0311.listen("TCP", ut0311.tcp)
		wg.Done()
	}()

	// ... start TLS listener
	wg.Add(1)
	go func() {
		ut0311.listen("TLS", ut0311.tls)
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

func (ut0311 *UT0311) listen(tag string, c listener) {
	delay := BACKOFF

	for {
		timer := time.AfterFunc(10*time.Second, func() {
			delay = BACKOFF
		})

		if err := c.listen(ut0311.received); err != nil {
			if !errors.Is(err, net.ErrClosed) || !c.isClosing() {
				warnf("%v", err)
			}
		}

		timer.Stop()

		if ut0311.closing {
			break
		}

		warnf("%v  reconnecting in %v", tag, delay)

		time.Sleep(delay)
		if dt := 2 * delay; dt > MAX_BACKOFF {
			delay = MAX_BACKOFF
		} else {
			delay = dt
		}
	}
}

func (ut0311 UT0311) received(request any) (any, error) {
	infof("UDP  request %T", request)

	if !ut0311.rateLimit.Allow() {
		return nil, fmt.Errorf("rate limit exceeded")
	}

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

	case *messages.GetCardsRequest:
		return ut0311.getCards(rq)

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
