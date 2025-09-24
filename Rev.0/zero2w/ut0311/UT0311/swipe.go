package UT0311

import (
	"fmt"
	"strconv"
	"time"

	"ut0311/entities"
	"ut0311/system"
)

func (u UT0311) Swipe(timestamp time.Time, controller uint32, card any, door uint8) {
	parse := func() (uint32, error) {
		switch v := card.(type) {
		case string:
			if c, err := strconv.ParseUint(v, 10, 32); err == nil {
				return uint32(c), nil
			}

		case uint32:
			return v, nil
		}

		return 0, fmt.Errorf("unknown type (%T)", card)
	}

	validate := func(record entities.Card) entities.EventReason {
		if timestamp.Before(record.StartDate) {
			return entities.ReasonCardDeniedNoAccess
		} else if timestamp.After(record.EndDate.Add(24 * time.Hour)) {
			return entities.ReasonCardDeniedNoAccess
		} else if permissions := record.Permissions[door]; permissions == 0 {
			return entities.ReasonCardDeniedNoAccess
		}

		return entities.ReasonCardOk
	}

	get := func(doors ...uint8) (bool, bool, error) {
		var open bool = false
		var unlocked bool = false
		var err error = nil

		for _, d := range doors {
			if v, e := u.state.DoorOpen(d); e != nil {
				err = e
			} else if v {
				open = true
			}

			if v, e := u.state.DoorUnlocked(d); e != nil {
				err = e
			} else if v {
				unlocked = true
			}
		}

		return open, unlocked, err
	}

	interlocked := func(interlock system.Interlock, door uint8) bool {
		doors := []uint8{}

		switch {
		case interlock == system.NoInterlock:
			doors = []uint8{}

		case interlock == system.Interlock12 && door == 1:
			doors = []uint8{2}

		case interlock == system.Interlock12 && door == 2:
			doors = []uint8{1}

		case interlock == system.Interlock34 && door == 3:
			doors = []uint8{4}

		case interlock == system.Interlock34 && door == 4:
			doors = []uint8{3}

		case interlock == system.Interlock12_34 && door == 1:
			doors = []uint8{2}

		case interlock == system.Interlock12_34 && door == 2:
			doors = []uint8{1}

		case interlock == system.Interlock12_34 && door == 3:
			doors = []uint8{4}

		case interlock == system.Interlock12_34 && door == 4:
			doors = []uint8{3}

		case interlock == system.Interlock123 && door == 1:
			doors = []uint8{2, 3}

		case interlock == system.Interlock123 && door == 2:
			doors = []uint8{1, 3}

		case interlock == system.Interlock123 && door == 3:
			doors = []uint8{1, 2}

		case interlock == system.Interlock1234 && door == 1:
			doors = []uint8{2, 3, 4}

		case interlock == system.Interlock1234 && door == 2:
			doors = []uint8{1, 3, 4}

		case interlock == system.Interlock1234 && door == 3:
			doors = []uint8{1, 2, 4}

		case interlock == system.Interlock1234 && door == 4:
			doors = []uint8{1, 2, 3}
		}

		if open, unlocked, err := get(doors...); open || unlocked || err != nil {
			return true
		}

		return false
	}

	antipassbacked := func(antipassback system.AntiPassback, door uint8, card uint32) bool {
		if swipe, err := u.system.GetSwipe(controller, card); err != nil {
			warnf("swipe: anti-passback error  controller:%v door:%v card:%v (%v)", controller, door, card, err)
		} else {
			doors := []uint8{}

			switch {
			case antipassback == system.NoAntiPassback:
				doors = []uint8{0, 1, 2, 3, 4}

			case antipassback == system.Readers12_34 && door == 1:
				doors = []uint8{0, 2}

			case antipassback == system.Readers12_34 && door == 2:
				doors = []uint8{0, 1}

			case antipassback == system.Readers12_34 && door == 3:
				doors = []uint8{0, 4}

			case antipassback == system.Readers12_34 && door == 4:
				doors = []uint8{0, 3}

			case antipassback == system.Readers13_24 && door == 1:
				doors = []uint8{0, 2, 4}

			case antipassback == system.Readers13_24 && door == 2:
				doors = []uint8{0, 1, 3}

			case antipassback == system.Readers13_24 && door == 3:
				doors = []uint8{0, 2, 4}

			case antipassback == system.Readers13_24 && door == 4:
				doors = []uint8{0, 1, 3}

			case antipassback == system.Readers1_23 && door == 1:
				doors = []uint8{0, 2, 3}

			case antipassback == system.Readers1_23 && door == 2:
				doors = []uint8{0, 1}

			case antipassback == system.Readers1_23 && door == 3:
				doors = []uint8{0, 1}

			case antipassback == system.Readers1_234 && door == 1:
				doors = []uint8{0, 2, 3, 4}

			case antipassback == system.Readers1_234 && door == 2:
				doors = []uint8{0, 1}

			case antipassback == system.Readers1_234 && door == 3:
				doors = []uint8{0, 1}

			case antipassback == system.Readers1_234 && door == 4:
				doors = []uint8{0, 1}
			}

			for _, d := range doors {
				if d == swipe.Door {
					return false
				}
			}

			return true
		}

		return false
	}

	denied := func(card uint32, reason entities.EventReason, err error) {
		warnf("swipe: access denied  controller:%v door:%v card:%v (%v)", controller, door, card, err)

		u.event(controller, &entities.Event{
			Index:     0,
			Type:      entities.EventCard,
			Granted:   false,
			Door:      door,
			Direction: 1,
			Card:      card,
			Timestamp: timestamp,
			Reason:    reason,
		})
	}

	granted := func(card uint32) {
		infof("swipe: access granted  controller:%v door:%v card:%v", controller, door, card)

		go func() {
			u.event(controller, &entities.Event{
				Index:     0,
				Type:      entities.EventCard,
				Granted:   true,
				Door:      door,
				Direction: 1,
				Card:      card,
				Timestamp: timestamp,
				Reason:    entities.ReasonCardOk,
			})
		}()

		go func() {
			if ok, err := u.system.PutSwipe(controller, door, card); err != nil {
				warnf("swipe: error updating 'swipes' list controller:%v door:%v card:%v (%v)", controller, door, card, err)
			} else if !ok {
				warnf("swipe: failed updating 'swipes' list  controller:%v door:%v card:%v", controller, door, card)
			}
		}()
	}

	if card, err := parse(); err == nil {
		infof("swipe  controller:%v door:%v card:%v", controller, door, card)

		if record, err := u.cards.GetCard(controller, card); err != nil {
			denied(card, entities.ReasonCardDenied, err)
		} else if reason := validate(record); reason != entities.ReasonCardOk {
			denied(card, reason, fmt.Errorf("%v", reason))
		} else if mode, _, err := u.system.GetDoor(controller, door); err != nil {
			denied(card, entities.ReasonUnknown, err)
		} else if mode == system.NormallyClosed {
			denied(card, entities.ReasonCardDeniedDoorNormallyClosed, err)
		} else if interlock, err := u.system.GetInterlock(controller); err != nil {
			denied(card, entities.ReasonUnknown, err)
		} else if interlocked(interlock, door) {
			denied(card, entities.ReasonCardDeniedDoorInterLock, fmt.Errorf("door interlock"))
		} else if antipassback, err := u.system.GetAntiPassback(controller); err != nil {
			denied(card, entities.ReasonUnknown, err)
		} else if antipassbacked(antipassback, door, card) {
			denied(card, entities.ReasonCardDeniedAntiPassback, fmt.Errorf("anti-passback"))
		} else if record.PIN != 0 {
			denied(card, entities.ReasonCardDeniedPassword, fmt.Errorf("*** PIN NOT IMPLEMENTED ***"))
		} else if ok, err := u.breakout.UnlockDoor(door); err != nil {
			denied(card, entities.ReasonUnknown, err)
		} else if !ok {
			denied(card, entities.ReasonUnknown, fmt.Errorf("error unlocking door"))
		} else {
			granted(card)

			tag := fmt.Sprintf("controller.door.%v.unlocked", door)
			u.state.Set(controller, tag, true)
		}

		return
	}

	warnf("invalid card swipe  controller:%v door:%v card:%v", controller, door, card)
}
