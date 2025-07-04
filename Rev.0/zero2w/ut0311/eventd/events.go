package eventd

import (
	"fmt"
	"net/rpc"
	"regexp"
	"sync/atomic"

	"ut0311/entities"
	"ut0311/log"
)

const LOGTAG = "EVENTD"

type Events struct {
	dial struct {
		network string
		address string
	}

	recordAll bool
	index     uint32
}

func NewEvents(dial string) (*Events, error) {
	infof("init dial:%v", dial)

	e := Events{
		recordAll: false,
	}

	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(dial); len(matches) < 3 {
		return nil, fmt.Errorf("invalid RPC 'dial' address (%v)", dial)
	} else {
		e.dial.network = matches[1]
		e.dial.address = matches[2]
	}

	return &e, nil
}

var Index atomic.Uint32

func (e *Events) Add(controller uint32, event entities.Event) (uint32, error) {
	debugf("add-event %v", event)

	var args = struct {
		Controller uint32
		Event      entities.Event
	}{
		Controller: controller,
		Event:      event,
	}

	var index uint32

	if client, err := rpc.DialHTTP(e.dial.network, e.dial.address); err != nil {
		return 0, err
	} else if err := client.Call("EventD.Add", args, &index); err != nil {
		return 0, err
	} else {
		return index, nil
	}
}

func (e *Events) Get(index uint32) (entities.Event, error) {
	debugf("get-event %v", index)

	var event entities.Event

	if client, err := rpc.DialHTTP(e.dial.network, e.dial.address); err != nil {
		return entities.Event{}, err
	} else if err := client.Call("EventD.Get", index, &event); err != nil {
		return entities.Event{}, err
	} else {
		return event, nil
	}
}

func (e *Events) GetEventIndex(controller uint32) (uint32, error) {
	debugf("get-event-index")

	var index uint32

	if client, err := rpc.DialHTTP(e.dial.network, e.dial.address); err != nil {
		return 0, err
	} else if err := client.Call("EventD.GetEventIndex", controller, &index); err != nil {
		return 0, err
	} else {
		return index, nil
	}
}

func (e *Events) SetEventIndex(controller uint32, index uint32) error {
	debugf("set-event-index")

	var args = struct {
		Controller uint32
		Index      uint32
	}{
		Controller: controller,
		Index:      index,
	}

	if client, err := rpc.DialHTTP(e.dial.network, e.dial.address); err != nil {
		return err
	} else if err := client.Call("EventD.SetEventIndex", args, nil); err != nil {
		return err
	} else {
		return nil
	}
}

func (e *Events) RecordSpecialEvents(controller uint32, enabled bool) error {
	debugf("record-special-events")

	var args = struct {
		Controller uint32
		Enabled    bool
	}{
		Controller: controller,
		Enabled:    enabled,
	}

	if client, err := rpc.DialHTTP(e.dial.network, e.dial.address); err != nil {
		return err
	} else if err := client.Call("EventD.RecordSpecialEvents", args, nil); err != nil {
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
