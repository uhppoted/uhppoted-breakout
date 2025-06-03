package eventd

import (
	"context"
	"errors"
	"fmt"
	"net"
	"net/http"
	"net/rpc"
	"os"
	"path/filepath"
	"regexp"

	"eventd/db"
	"eventd/entities"
	"eventd/log"
)

const LOGTAG = "eventd"

type EventD struct {
	ctx    context.Context
	cancel context.CancelFunc

	listen struct {
		network string
		addr    string
	}
}

func NewEventD(listen string) (*EventD, error) {
	ctx, cancel := context.WithCancel(context.Background())

	v := EventD{
		ctx:    ctx,
		cancel: cancel,
	}

	// ... set 'listen' address:port
	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(listen); len(matches) < 3 {
		return nil, fmt.Errorf("invalid bind address (%v)", listen)
	} else {
		v.listen = struct {
			network string
			addr    string
		}{
			network: matches[1],
			addr:    matches[2],
		}
	}

	return &v, nil
}

func (d *EventD) Run() {
	rpc.Register(d)
	rpc.HandleHTTP()

	if d.listen.network == "unix" {
		folder := filepath.Dir(d.listen.addr)
		if err := os.MkdirAll(folder, 0766); err != nil {
			errorf("listen error: %v", err)
		}

		os.Remove(d.listen.addr)
	}

	if l, err := net.Listen(d.listen.network, d.listen.addr); err != nil {
		errorf("listen error: %v", err)
	} else {
		go func() {
			<-d.ctx.Done()
			l.Close()
		}()

		infof("listening %v %v", d.listen.network, d.listen.addr)
		http.Serve(l, nil)
	}
}

func (d *EventD) Stop() {
	d.cancel()
}

func (d *EventD) Add(event entities.Event, reply *uint32) error {
	debugf("add-event %v", event)

	if index, err := db.PutEvent(event); err != nil {
		return err
	} else {
		*reply = index

		return nil
	}
}

func (d *EventD) Get(index uint32, event *entities.Event) error {
	debugf("get-event %v", index)

	get := func(ix uint32) error {
		if record, err := db.GetEvent(ix); err != nil {
			return err
		} else {
			*event = record
		}

		return nil
	}

	if err := get(index); err != nil && !errors.Is(err, entities.ErrRecordNotFound) {
		return err
	} else if err == nil {
		return nil
	}

	if first, last, err := db.GetEvents(); err != nil {
		return err
	} else {
		switch {
		case index == 0:
			return get(first)

		case index == 0xffffffff:
			return get(last)

		case index < first:
			*event = entities.Event{
				Index: index,
				Type:  0xff, // overwritten
			}

			return nil

		case index > last:
			return get(last)

		default:
			return entities.ErrRecordNotFound
		}
	}
}

func (d *EventD) GetEventIndex(controller uint32, index *uint32) error {
	debugf("get-event-index %v", controller)

	if record, err := db.GetEventIndex(controller); err != nil {
		return err
	} else {
		*index = record
	}

	return nil
}

func (d *EventD) SetEventIndex(args struct {
	Controller uint32
	Index      uint32
}, _ *any) error {
	debugf("set-event-index %v %v", args.Controller, args.Index)

	if err := db.SetEventIndex(args.Controller, args.Index); err != nil {
		return err
	} else {
		return nil
	}
}

func debugf(format string, args ...any) {
	log.Debugf(LOGTAG, format, args...)
}

func infof(format string, args ...any) {
	log.Infof(LOGTAG, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(LOGTAG, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(LOGTAG, format, args...)
}
