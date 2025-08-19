package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) deleteCard(rq *messages.DeleteCardRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if ok, err := ut0311.cards.DeleteCard(controller, rq.CardNumber); err != nil {
		warnf("delete-card controller:%v card:%v (%v)", controller, rq.CardNumber, err)

		return nil, nil
	} else {
		return messages.DeleteCardResponse{
			SerialNumber: types.SerialNumber(controller),
			Succeeded:    ok,
		}, nil
	}
}
