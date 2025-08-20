package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) deleteAllCards(rq *messages.DeleteCardsRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if rq.MagicWord != 0x55aaaa55 {
		return nil, fmt.Errorf("not authorised")
	} else if ok, err := ut0311.cards.DeleteAllCards(controller); err != nil {
		return nil, err
	} else {
		return messages.DeleteCardsResponse{
			SerialNumber: types.SerialNumber(controller),
			Succeeded:    ok,
		}, nil
	}
}
