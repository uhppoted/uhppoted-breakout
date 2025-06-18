package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) deleteCard(rq *messages.DeleteCardRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.DeleteCardResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if deleted, err := scmp.DeleteIndexed[scmp.Card](ut0311.cards, scmp.OID_CARDS_CARD, rq.CardNumber); err != nil {
			return nil, err
		} else {
			response.Succeeded = deleted
		}

		return response, nil
	}
}
