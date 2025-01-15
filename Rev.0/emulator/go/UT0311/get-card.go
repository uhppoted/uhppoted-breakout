package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) getCard(rq *messages.GetCardByIDRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetCardByIDResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if card, err := scmp.GetIndexed[scmp.Card](ut0311.cards, scmp.OID_CARDS_CARD, rq.CardNumber); err != nil {
			return nil, err
		} else {
			response.CardNumber = card.Card
			response.From = card.StartDate
			response.To = card.EndDate
			response.Door1 = card.Permissions[1]
			response.Door2 = card.Permissions[2]
			response.Door3 = card.Permissions[3]
			response.Door4 = card.Permissions[4]
			response.PIN = types.PIN(card.PIN)
		}

		return response, nil
	}
}
