package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) getCards(rq *messages.GetCardsRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetCardsResponse{
			SerialNumber: types.SerialNumber(id),
			Records:      0,
		}

		if N, err := scmp.Get[uint32](ut0311.cards, scmp.OID_CARDS_COUNT); err != nil {
			return nil, err
		} else {
			response.Records = N
		}

		return response, nil
	}
}
