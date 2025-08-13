package actions

import (
	"strconv"
	"time"

	"ut0311/cards"
	"ut0311/entities"
)

func Swipe(timestamp time.Time, controller uint32, card any, door uint8, db *cards.Cards) *entities.Event {
	switch v := card.(type) {
	case string:
		if c, err := strconv.ParseUint(v, 10, 32); err == nil {
			return swipe(timestamp, controller, uint32(c), door, db)
		}

	case uint32:
		return swipe(timestamp, controller, v, door, db)
	}

	warnf("CARD", "invalid card swipe  controller:%v, card:%v, door:%v,", controller, card, door)

	return nil
}

func swipe(timestamp time.Time, controller uint32, card uint32, door uint8, db *cards.Cards) *entities.Event {
	infof("CARD", "swipe  controller:%v, card:%v, door:%v,", controller, card, door)

	validate := func(record entities.Card) entities.EventReason {
		if timestamp.Before(record.StartDate) {
			return entities.ReasonCardDeniedNoAccess
		}

		if timestamp.After(record.EndDate.Add(24 * time.Hour)) {
			return entities.ReasonCardDeniedNoAccess
		}

		if permissions := record.Permissions[door]; permissions == 0 {
			return entities.ReasonCardDeniedNoAccess
		}
		if record.PIN != 0 {
			return entities.ReasonCardDeniedPassword
		}

		return entities.ReasonCardOk
	}

	if record, err := db.GetCard(controller, card); err != nil {
		warnf("CARD", "access denied  controller:%v card:%v door:%v (%v)", controller, card, door, err)

		return &entities.Event{
			Index:     0,
			Type:      entities.EventCard,
			Granted:   false,
			Door:      door,
			Direction: 1,
			Card:      card,
			Timestamp: timestamp,
			Reason:    entities.ReasonCardDenied,
		}
	} else if reason := validate(record); reason != entities.ReasonCardOk {
		warnf("CARD", "access denied  controller:%v card:%v door:%v", controller, card, door)

		return &entities.Event{
			Index:     0,
			Type:      entities.EventCard,
			Granted:   false,
			Door:      door,
			Direction: 1,
			Card:      card,
			Timestamp: timestamp,
			Reason:    reason,
		}
	} else {
		infof("CARD", "access granted  controller:%v card:%v door:%v", controller, card, door)

		return &entities.Event{
			Index:     0,
			Type:      entities.EventCard,
			Granted:   true,
			Door:      door,
			Direction: 1,
			Card:      card,
			Timestamp: timestamp,
			Reason:    entities.ReasonCardOk,
		}
	}

	return nil
}
