package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) deleteAllCards(rq *messages.DeleteCardsRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if rq.MagicWord != 0x55aaaa55 {
		return nil, fmt.Errorf("not authorised")
	} else {
		response := messages.DeleteCardsResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if deleted, err := scmp.DeleteIndexed[scmp.Card](ut0311.cards, scmp.OID_CARDS_CARD, 0xffffffff); err != nil {
			return nil, err
		} else {
			response.Succeeded = deleted
		}

		return response, nil
	}
}
