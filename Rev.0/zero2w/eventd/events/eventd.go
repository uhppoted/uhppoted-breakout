package events

import (
	"errors"

	"eventd/db"
	"eventd/entities"
	"eventd/log"
)

const LOGTAG = "eventd"

type EventD struct {
}

func (d *EventD) Add(args struct {
	Controller uint32
	Event      entities.Event
}, reply *uint32) error {
	debugf("add-event %v %v", args.Controller, args.Event)

	if index, err := db.PutEvent(args.Controller, args.Event); err != nil {
		warnf("add-event (%v)", err)
		return err
	} else {
		infof("add-event stored %v @%v", args.Event, index)

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
		warnf("get-event (%v)", err)
		return err
	} else if err == nil {
		return nil
	}

	if first, last, err := db.GetEvents(); err != nil {
		warnf("get-event (%v)", err)
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
		warnf("get-event-index (%v)", err)
		return err
	} else {
		*index = record
	}

	return nil
}

func (d *EventD) SetEventIndex(args struct {
	Controller uint32
	Index      uint32
}, _ *uint32) error {
	debugf("set-event-index %v %v", args.Controller, args.Index)

	if err := db.SetEventIndex(args.Controller, args.Index); err != nil {
		warnf("set-event-index (%v)", err)
		return err
	}

	return nil
}

func (d *EventD) RecordSpecialEvents(args struct {
	Controller uint32
	Enabled    bool
}, _ *bool) error {
	debugf("record-special-events %v %v", args.Controller, args.Enabled)

	if err := db.RecordSpecialEvents(args.Controller, args.Enabled); err != nil {
		warnf("record-special-events (%v)", err)
		return err
	}

	return nil
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
