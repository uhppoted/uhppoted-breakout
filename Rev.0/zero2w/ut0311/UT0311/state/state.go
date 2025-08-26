package state

import (
	"fmt"
	"sync"
	"time"

	"ut0311/entities"
	"ut0311/log"
)

type State struct {
	state map[string]any

	wg   sync.WaitGroup
	done chan bool
}

func NewState() *State {
	return &State{
		state: map[string]any{
			"controller.system.errors.restart":  false,
			"controller.system.errors.watchdog": false,
			"controller.system.special-info":    uint8(0),
			"controller.system.sequence-number": uint32(0),

			"controller.door.1.open": false,
			"controller.door.2.open": false,
			"controller.door.3.open": false,
			"controller.door.4.open": false,

			"controller.door.1.button": false,
			"controller.door.2.button": false,
			"controller.door.3.button": false,
			"controller.door.4.button": false,

			"controller.door.1.unlocked": false,
			"controller.door.2.unlocked": false,
			"controller.door.3.unlocked": false,
			"controller.door.4.unlocked": false,
		},

		done: make(chan bool),
	}
}

func (s *State) Poll() {
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
			// println(" >>> poll")
		}
	}
}

func (s *State) Stop() error {
	s.done <- true
	s.wg.Wait()

	return nil
}

func (s *State) Set(timestamp time.Time, controller uint32, tag string, value any) {
	debugf("set %v %v %v", timestamp, tag, value)

	s.state[tag] = value
}

func (s *State) Update(timestamp time.Time, controller uint32, tag string, value any) *entities.Event {
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

func (s *State) DateTime() (time.Time, error) {
	return time.Now(), nil
}

func (s *State) SystemError(flags ...uint16) (bool, error) {
	tags := map[uint16]string{
		entities.ErrMemory:   "controller.system.error.memory",
		entities.ErrQueue:    "controller.system.error.queue",
		entities.ErrI2C:      "controller.system.error.I2C",
		entities.ErrRX8900:   "controller.system.error.RX8900",
		entities.ErrU2:       "controller.system.error.U2",
		entities.ErrU3:       "controller.system.error.U3",
		entities.ErrU4:       "controller.system.error.U4",
		entities.ErrWatchdog: "controller.system.error.watchdog",
		entities.ErrDebug:    "controller.system.error.debug",
		entities.ErrUnknown:  "controller.system.error.unknown ",
	}

	for _, f := range flags {
		if tag, ok := tags[f]; ok {
			if v, ok := s.state[tag]; ok {
				if err, ok := v.(bool); ok && err {
					return true, nil
				}
			}
		}
	}

	return false, nil
}

func (s *State) SpecialInfo() (uint8, error) {
	if v, ok := s.state["controller.system.special-info"]; !ok {
		return 0, fmt.Errorf("controller.system.special-info not cached")
	} else if u8, ok := v.(uint8); !ok {
		return 0, fmt.Errorf("controller.system.special-info invalid value")
	} else {
		return u8, nil
	}
}

func (s *State) SequenceNo() (uint32, error) {
	if v, ok := s.state["controller.system.sequence-number"]; !ok {
		return 0, fmt.Errorf("controller.system.sequence-number not cached")
	} else if u32, ok := v.(uint32); !ok {
		return 0, fmt.Errorf("controller.system.sequence-number invalid value")
	} else {
		return u32, nil
	}
}

func (s *State) DoorOpen(door uint8) (bool, error) {
	tag := fmt.Sprintf("controller.door.%v.open", door)

	if v, ok := s.state[tag]; !ok {
		return false, fmt.Errorf("%v not cached", tag)
	} else if b, ok := v.(bool); !ok {
		return false, fmt.Errorf("%v invalid value", tag)
	} else {
		return b, nil
	}
}

func (s *State) DoorButton(door uint8) (bool, error) {
	tag := fmt.Sprintf("controller.door.%v.button", door)

	if v, ok := s.state[tag]; !ok {
		return false, fmt.Errorf("%v not cached", tag)
	} else if b, ok := v.(bool); !ok {
		return false, fmt.Errorf("%v invalid value", tag)
	} else {
		return b, nil
	}
}

func (s *State) DoorUnlocked(door uint8) (bool, error) {
	tag := fmt.Sprintf("controller.door.%v.unlocked", door)

	if v, ok := s.state[tag]; !ok {
		return false, fmt.Errorf("%v not cached", tag)
	} else if b, ok := v.(bool); !ok {
		return false, fmt.Errorf("%v invalid value", tag)
	} else {
		return b, nil
	}
}

func debugf(format string, args ...any) {
	log.Debugf("state", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("state", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("state", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("state", format, args...)
}
