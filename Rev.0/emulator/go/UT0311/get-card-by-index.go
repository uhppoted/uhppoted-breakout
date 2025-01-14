package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) getCardByIndex(rq *messages.GetCardByIndexRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetCardByIndexResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if index := rq.Index; index > 0 {
			if card, err := scmp.GetIndexed[uint32](ut0311.cards, scmp.OID_CARDS_CARD_NUMBER, index); err != nil {
				return nil, err
			} else if card == 0 {
				return nil, fmt.Errorf("card not found")

			} else {
				response.CardNumber = card
			}

			if date, err := scmp.GetIndexed[types.Date](ut0311.cards, scmp.OID_CARDS_CARD_START_DATE, index); err != nil {
				return nil, err
			} else {
				response.From = date
			}

			if date, err := scmp.GetIndexed[types.Date](ut0311.cards, scmp.OID_CARDS_CARD_END_DATE, index); err != nil {
				return nil, err
			} else {
				response.To = date
			}

			if permission, err := scmp.GetIndexed[uint8](ut0311.cards, scmp.OID_CARDS_CARD_DOOR1, index); err != nil {
				return nil, err
			} else {
				response.Door1 = permission
			}

			if permission, err := scmp.GetIndexed[uint8](ut0311.cards, scmp.OID_CARDS_CARD_DOOR2, index); err != nil {
				return nil, err
			} else {
				response.Door2 = permission
			}

			if permission, err := scmp.GetIndexed[uint8](ut0311.cards, scmp.OID_CARDS_CARD_DOOR3, index); err != nil {
				return nil, err
			} else {
				response.Door3 = permission
			}

			if permission, err := scmp.GetIndexed[uint8](ut0311.cards, scmp.OID_CARDS_CARD_DOOR4, index); err != nil {
				return nil, err
			} else {
				response.Door4 = permission
			}

			if PIN, err := scmp.GetIndexed[uint32](ut0311.cards, scmp.OID_CARDS_CARD_PIN, index); err != nil {
				return nil, err
			} else {
				response.PIN = types.PIN(PIN)
			}
		}

		return response, nil
	}
}
