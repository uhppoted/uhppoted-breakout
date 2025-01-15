package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) putCard(rq *messages.PutCardRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.PutCardResponse{
			SerialNumber: types.SerialNumber(id),
		}

		card := scmp.Card{
			Card:      rq.CardNumber,
			StartDate: rq.From,
			EndDate:   rq.To,
			Permissions: map[uint8]uint8{
				1: rq.Door1,
				2: rq.Door2,
				3: rq.Door3,
				4: rq.Door4,
			},
			PIN: uint32(rq.PIN),
		}

		if _, err := scmp.SetIndexed[scmp.Card](&ut0311.cards, scmp.OID_CARDS_CARD, rq.CardNumber, card); err != nil {
			return nil, err
		} else {
			response.Succeeded = true
		}

		return response, nil
	}
}
