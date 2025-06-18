package UT0311

import (
	"context"
	"errors"
	"fmt"
	"net"
	"reflect"
	"sync"
	"time"

	"golang.org/x/time/rate"

	"github.com/uhppoted/uhppote-core/messages"

	"ut0311/cards"
	"ut0311/config"
	"ut0311/driver/rpcd"
	"ut0311/events"
	"ut0311/log"
	"ut0311/system"
)

const BACKOFF = 500 * time.Millisecond
const MAX_BACKOFF = 60 * time.Second
const REQUEST_RATE_LIMIT = 5.0   // requests/second
const REQUEST_BURST_LIMIT = 10.0 // burst requests

type listener interface {
	listen(received func(any) (any, error)) error
	stop() error
	isClosing() bool
}

type UT0311 struct {
	config   *config.Config
	breakout *rpcd.RPC
	system   system.System
	events   *events.Events
	cards    *cards.Cards

	cm   *ConnectionManager
	udp  *UDP
	tcp  *TCP
	tls  *TLS
	rate *rate.Limiter

	state *state

	closing bool
}

func (ut0311 *UT0311) SetConfig(c *config.Config) {
	ut0311.config = c
}

func NewUT0311(c *config.Config) (*UT0311, error) {
	cm := NewConnectionManager(MAX_CONNECTIONS)

	ut0311 := UT0311{
		config: c,
		system: system.System{},
		cards:  &cards.Cards{},

		cm:   cm,
		udp:  makeUDP(cm),
		tcp:  makeTCP(cm),
		tls:  makeTLS(c.TLS.Certificate, c.TLS.CA, cm),
		rate: rate.NewLimiter(REQUEST_RATE_LIMIT, REQUEST_BURST_LIMIT),

		state: &state{},

		closing: false,
	}

	if rpc, err := rpcd.NewRPC(c.Driver.RPC.DialAddr, c.Driver.RPC.ListenAddr, c.Driver.Caching, ut0311.onTrap); err != nil {
		return nil, err
	} else {
		ut0311.breakout = rpc
	}

	if eventd, err := events.NewEvents(c.Events.RPC.DialAddr); err != nil {
		return nil, err
	} else {
		ut0311.events = eventd
	}

	return &ut0311, nil
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

	// ... start SSMP event trap listener
	wg.Add(1)
	go func() {
		ut0311.breakout.Listen()
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

	wg.Add(1)
	go func() {
		infof("closing active connections")
		if err := ut0311.cm.close(); err != nil {
			warnf("%v", err)
		}

		wg.Done()
	}()

	wg.Add(1)
	go func() {
		infof("stopping SSMP event handler")
		if err := ut0311.breakout.Stop(); err != nil {
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
	infof("UDP  request %T tokens:%v", request, ut0311.rate.Tokens())

	if !ut0311.rate.Allow() {
		return nil, fmt.Errorf("request rate limit exceeded")
	}

	ctx, cancel := context.WithDeadline(context.Background(), time.Now().Add(5*time.Second))
	defer cancel()

	result, err := ut0311.dispatch(ctx, request)

	select {
	case <-ctx.Done():
		return nil, fmt.Errorf("timeout")

	default:
		return result, err
	}
}

func (ut0311 UT0311) dispatch(ctx context.Context, request any) (any, error) {
	infof("UDP  dispatch %T", request)

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

	case *messages.OpenDoorRequest:
		return ut0311.openDoor(rq)

	case *messages.GetCardsRequest:
		return ut0311.getCards(rq)

	case *messages.GetCardByIDRequest:
		return ut0311.getCard(rq)

	case *messages.GetCardByIndexRequest:
		return ut0311.getCardByIndex(rq)

	case *messages.PutCardRequest:
		return ut0311.putCard(rq)

	case *messages.DeleteCardRequest:
		return ut0311.deleteCard(rq)

	case *messages.DeleteCardsRequest:
		return ut0311.deleteAllCards(rq)

	case *messages.RecordSpecialEventsRequest:
		return ut0311.recordSpecialEvents(rq)

	case *messages.GetEventIndexRequest:
		return ut0311.getEventIndex(rq)

	case *messages.SetEventIndexRequest:
		return ut0311.setEventIndex(rq)

	case *messages.GetEventRequest:
		return ut0311.getEvent(rq)

	default:
		warnf("unknown message type (%T)", request)
	}

	return nil, nil
}

func (ut0311 *UT0311) onTrap(controller uint32, timestamp time.Time, tag string, value any) {
	debugf("trap %v %v %v %v", controller, timestamp, tag, value)

	ut0311.state.update(timestamp, tag, value)
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
