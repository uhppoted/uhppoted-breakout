package actions

import (
	"fmt"
	"strconv"
	"time"

	"ut0311/entities"
	"ut0311/eventd"
)

func Swipe(timestamp time.Time, controller uint32, card any, door uint8, db *eventd.Events) *entities.Event {
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

func swipe(timestamp time.Time, controller uint32, card uint32, door uint8, db *eventd.Events) *entities.Event {
	infof("CARD", "swipe  controller:%v, card:%v, door:%v,", controller, card, door)

	if record, err := db.GetCard(controller, card); err != nil {
		warnf("CARD", "access denied  controller:%v, card:%v, door:%v (%v)", controller, card, door, err)

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
	} else {
		fmt.Printf(">>>>>>>> SWIPE/OK %v %v %v\n,", card, door, record)
	}

	return nil
}
