package actions

import (
	"fmt"
	"strconv"

	"ut0311/eventd"
)

func Swipe(db *eventd.Events, card any, door uint8) {
	switch v := card.(type) {
	case string:
		if c, err := strconv.ParseUint(v, 10, 32); err == nil {
			swipe(db, uint32(c), door)
		}

	case uint32:
		swipe(db, v, door)
	}
}

func swipe(db *eventd.Events, card uint32, door uint8) {
	fmt.Printf(">>>>>>>> SWIPE %v %v\n,", card, door)

	if record, err := db.GetCard(card); err != nil {
		fmt.Printf(">>>>>>>> SWIPE/ERROR %v %v %v\n,", card, door, err)
	} else {
		fmt.Printf(">>>>>>>> SWIPE/OK %v %v %v\n,", card, door, record)
	}

}
