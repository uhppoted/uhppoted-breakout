package UT0311

import (
	"sync"
	"time"

	"ut0311/entities"
)

type state struct {
	state map[string]any

	wg   sync.WaitGroup
	done chan bool
}

func newState() *state {
	return &state{
		state: map[string]any{
			"controller.system.errors.restart":  false,
			"controller.system.errors.watchdog": false,

			"controller.door.1.open": false,
			"controller.door.2.open": false,
			"controller.door.3.open": false,
			"controller.door.4.open": false,

			"controller.door.1.button": false,
			"controller.door.2.button": false,
			"controller.door.3.button": false,
			"controller.door.4.button": false,
		},

		done: make(chan bool),
	}
}

func (s *state) poll() {
	s.wg.Add(1)

	defer func() {
		s.wg.Done()
	}()

	tick := time.Tick(5 * time.Second)

loop:
	for {
		select {
		case <-s.done:
			break loop

		case <-tick:
			println(" >>> poll")
		}
	}
}

func (s *state) stop() error {
	s.done <- true
	s.wg.Wait()

	return nil
}

func (s *state) update(timestamp time.Time, controller uint32, tag string, value any) *entities.Event {
	debugf("update %v %v %v", timestamp, tag, value)

	if old, ok := s.state[tag]; !ok {
		s.state[tag] = value
	} else if old != value {
		s.state[tag] = value

		if t := entities.LookupEvent(tag, value); t != entities.EventUnknown {
			return &entities.Event{
				Index:     0,
				Type:      t,
				Granted:   false,
				Door:      entities.LookupEventDoor(tag),
				Direction: 0,
				Card:      0,
				Timestamp: timestamp,
				Reason:    entities.LookupEventReason(tag, value),
			}
		}
	}

	return nil
}
