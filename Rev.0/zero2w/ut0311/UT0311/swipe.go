package UT0311

import (
	"fmt"
	"strconv"
	"time"

	"ut0311/entities"
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
		} else if record.PIN != 0 {
			return entities.ReasonCardDeniedPassword
		}

		return entities.ReasonCardOk
	}

	denied := func(card uint32, reason entities.EventReason, err error) {
		warnf("swipe: access denied  controller:%v card:%v door:%v (%v)", controller, card, door, err)

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
		infof("swipe: access granted  controller:%v card:%v door:%v", controller, card, door)

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
	}

	if card, err := parse(); err == nil {
		infof("swipe  controller:%v, card:%v, door:%v,", controller, card, door)

		if record, err := u.cards.GetCard(controller, card); err != nil {
			denied(card, entities.ReasonCardDenied, err)
		} else if reason := validate(record); reason != entities.ReasonCardOk {
			denied(card, reason, fmt.Errorf("card access permissions"))
		} else if ok, err := u.breakout.UnlockDoor(door); err != nil || !ok {
			denied(card, entities.ReasonUnknown, fmt.Errorf("error unlocking door"))
		} else {
			granted(card)

			tag := fmt.Sprintf("controller.door.%v.unlocked", door)
			u.state.Set(controller, tag, true)

			return
		}
	}

	warnf("invalid card swipe  controller:%v, card:%v, door:%v,", controller, card, door)
}
