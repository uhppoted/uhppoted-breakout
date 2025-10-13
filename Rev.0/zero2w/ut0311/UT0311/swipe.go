package UT0311

import (
	"fmt"
	"slices"
	"strconv"
	"sync"
	"sync/atomic"
	"time"

	"ut0311/entities"
	"ut0311/system"
)

const pinTimeout = 2500 * time.Millisecond

type swiped struct {
	ID         uint32
	controller uint32
	door       uint8
	card       uint32
	pin        uint32
	code       *string
	timer      *time.Timer
}

var swipes = struct {
	pending sync.Map
	ID      atomic.Uint32
}{
	pending: sync.Map{},
	ID:      atomic.Uint32{},
}

func (u UT0311) swipe(timestamp time.Time, controller uint32, door uint8, card any) {
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
		if record.Card == 0 {
			warnf("card %v - no record")
			return entities.ReasonCardDeniedNoAccess
		} else if timestamp.Before(record.StartDate) {
			warnf("card %v - invalid start date (%v)", record.Card, record.StartDate)
			return entities.ReasonCardDeniedNoAccess
		} else if timestamp.After(record.EndDate.Add(24 * time.Hour)) {
			warnf("card %v - invalid end date (%v)", record.Card, record.EndDate)
			return entities.ReasonCardDeniedNoAccess
		} else if permissions := record.Permissions[door]; permissions == 0 {
			warnf("card %v - invalid permissions (%v)", record.Card, record.Permissions)
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

	if card, err := parse(); err == nil {
		infof("swipe  controller:%v door:%v card:%v", controller, door, card)

		key := fmt.Sprintf("%v.%v", controller, door)

		if record, err := u.cards.GetCard(controller, card); err != nil {
			u.denied(controller, door, card, entities.ReasonCardDenied, err)
		} else if reason := validate(record); reason != entities.ReasonCardOk {
			u.denied(controller, door, card, reason, fmt.Errorf("%v", reason))
		} else if mode, _, err := u.system.GetDoor(controller, door); err != nil {
			u.denied(controller, door, card, entities.ReasonUnknown, err)
		} else if mode == system.NormallyClosed {
			u.denied(controller, door, card, entities.ReasonCardDeniedDoorNormallyClosed, err)
		} else if interlock, err := u.system.GetInterlock(controller); err != nil {
			u.denied(controller, door, card, entities.ReasonUnknown, err)
		} else if interlocked(interlock, door) {
			u.denied(controller, door, card, entities.ReasonCardDeniedDoorInterLock, fmt.Errorf("door interlock"))
		} else if antipassback, err := u.system.GetAntiPassback(controller); err != nil {
			u.denied(controller, door, card, entities.ReasonUnknown, err)
		} else if antipassbacked(antipassback, door, card) {
			u.denied(controller, door, card, entities.ReasonCardDeniedAntiPassback, fmt.Errorf("anti-passback"))
		} else {
			if v, ok := swipes.pending.LoadAndDelete(key); ok {
				if p, ok := v.(swiped); ok {
					p.timer.Stop()
					// NTS: rethink this!
					//      Sending a 'denied' here causes all kinds of weirdness e.g. unexpected 'denied' messages in the
					//      logs, reader access denied indications, etc.
					//
					// u.denied(controller, door, p.card, entities.ReasonCardDeniedPassword, fmt.Errorf("expecting PIN, got swipe"))
				}
			}

			code := ""

			if record.PIN != 0 {
				record := swiped{
					ID:         swipes.ID.Add(1),
					controller: controller,
					door:       door,
					card:       card,
					pin:        record.PIN,
					code:       &code,
				}

				record.timer = time.AfterFunc(pinTimeout, func() {
					u.denied(controller, door, card, entities.ReasonCardDeniedPassword, fmt.Errorf("PIN timeout"))
				})

				swipes.pending.Store(key, record)
			} else {
				u.unlock(controller, door, card)
			}
		}

		return
	}

	warnf("invalid card swipe  controller:%v door:%v card:%v", controller, door, card)
}

func (u UT0311) keyCode(timestamp time.Time, controller uint32, door uint8, code any) {
	debugf("keycode controller:%v, door:%v, code:%v", controller, door, code)
	// k := fmt.Sprintf("%v.%v", controller, door)
	// v, _ := pending.LoadAndDelete(k)
	//
	// if p, ok := v.(swiped); ok {
	// 	p.timer.Stop()
	// 	if code == fmt.Sprintf("%v#", p.pin) {
	// 		if ok, err := u.breakout.UnlockDoor(door); err != nil {
	// 			u.denied(p.controller, p.door, p.card, entities.ReasonUnknown, err)
	// 		} else if !ok {
	// 			u.denied(p.controller, p.door, p.card, entities.ReasonUnknown, fmt.Errorf("error unlocking door"))
	// 		} else {
	// 			u.granted(p.controller, p.door, p.card)
	// 		}
	// 	}
	// }
}

func (u UT0311) keyPress(timestamp time.Time, controller uint32, door uint8, digit any) {
	digits := []int64{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '#'}
	key := fmt.Sprintf("%v.%v", controller, door)

	if i64, ok := digit.(int64); ok && slices.Contains(digits, i64) {
		v := rune(i64)
		if record, ok := swipes.pending.Load(key); ok {
			if p, ok := record.(swiped); ok {
				if v == '*' || v == '#' {
					p.timer.Stop()

					swipes.pending.Delete(key)

					if code, err := strconv.ParseUint(*p.code, 10, 32); err == nil && uint32(code) == p.pin {
						u.unlock(p.controller, p.door, p.card)
					} else {
						u.denied(p.controller, p.door, p.card, entities.ReasonCardDeniedPassword, fmt.Errorf("incorrect PIN %v", code))
					}
				} else {
					*p.code = *p.code + string(v)

					p.timer.Reset(pinTimeout)
				}
			}
		}
	}
}

func (u UT0311) granted(controller uint32, door uint8, card uint32) {
	infof("swipe: access granted  controller:%v door:%v card:%v", controller, door, card)

	timestamp := time.Now()
	tag := fmt.Sprintf("controller.door.%v.unlocked", door)

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

	go func() {
		u.breakout.Granted(controller, door)
	}()

	u.state.Set(controller, tag, true)
}

func (u UT0311) denied(controller uint32, door uint8, card uint32, reason entities.EventReason, err error) {
	warnf("swipe: access denied  controller:%v door:%v card:%v (%v)", controller, door, card, err)

	timestamp := time.Now()

	go func() {

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
	}()

	go func() {
		u.breakout.Denied(controller, door)
	}()
}

func (u UT0311) unlock(controller uint32, door uint8, card uint32) {
	if ok, err := u.breakout.UnlockDoor(door); err != nil {
		u.denied(controller, door, card, entities.ReasonUnknown, err)
	} else if !ok {
		u.denied(controller, door, card, entities.ReasonUnknown, fmt.Errorf("error unlocking door"))
	} else {
		u.granted(controller, door, card)
	}
}
