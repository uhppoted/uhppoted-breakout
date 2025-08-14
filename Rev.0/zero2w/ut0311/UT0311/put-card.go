package UT0311

import (
	"time"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/entities"
	"ut0311/scmp"
)

func (ut0311 *UT0311) putCard(rq *messages.PutCardRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else {
		card := entities.Card{
			Card:      rq.CardNumber,
			StartDate: time.Time(rq.From),
			EndDate:   time.Time(rq.To),
			Permissions: map[uint8]uint8{
				1: rq.Door1,
				2: rq.Door2,
				3: rq.Door3,
				4: rq.Door4,
			},
			PIN: uint32(rq.PIN),
		}

		err := ut0311.cards.PutCard(controller, card)
		if err != nil {
			warnf("put-card failed (%v)", err)
		}

		return messages.PutCardResponse{
			SerialNumber: types.SerialNumber(controller),
			Succeeded:    err == nil,
		}, nil
	}
}
