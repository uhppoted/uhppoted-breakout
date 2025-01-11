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

		if index, err := scmp.GetIndexed[uint32](ut0311.cards, scmp.OID_CARDS_CARD, rq.CardNumber); err != nil {
			return nil, err
		} else if index > 0 {
			response.CardNumber = rq.CardNumber

			if date, err := scmp.GetIndexed[types.Date](ut0311.cards, scmp.OID_CARDS_CARD_START_DATE, rq.CardNumber); err != nil {
				return nil, err
			} else {
				response.From = date
			}

			if date, err := scmp.GetIndexed[types.Date](ut0311.cards, scmp.OID_CARDS_CARD_END_DATE, rq.CardNumber); err != nil {
				return nil, err
			} else {
				response.To = date
			}

			if permission, err := scmp.GetIndexed[uint8](ut0311.cards, scmp.OID_CARDS_CARD_DOOR1, rq.CardNumber); err != nil {
				return nil, err
			} else {
				response.Door1 = permission
			}

			if permission, err := scmp.GetIndexed[uint8](ut0311.cards, scmp.OID_CARDS_CARD_DOOR2, rq.CardNumber); err != nil {
				return nil, err
			} else {
				response.Door2 = permission
			}

			if permission, err := scmp.GetIndexed[uint8](ut0311.cards, scmp.OID_CARDS_CARD_DOOR3, rq.CardNumber); err != nil {
				return nil, err
			} else {
				response.Door3 = permission
			}

			if permission, err := scmp.GetIndexed[uint8](ut0311.cards, scmp.OID_CARDS_CARD_DOOR4, rq.CardNumber); err != nil {
				return nil, err
			} else {
				response.Door4 = permission
			}

			if PIN, err := scmp.GetIndexed[uint32](ut0311.cards, scmp.OID_CARDS_CARD_PIN, rq.CardNumber); err != nil {
				return nil, err
			} else {
				response.PIN = types.PIN(PIN)
			}
		}

		return response, nil
	}
}
