package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) getCard(rq *messages.GetCardByIDRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if record, err := ut0311.cards.GetCard(controller, rq.CardNumber); err != nil {
		warnf("get-card controller:%v card:%v (%v)", controller, rq.CardNumber, err)

		return messages.GetCardByIDResponse{
			SerialNumber: types.SerialNumber(controller),
			CardNumber:   0,
		}, nil
	} else {
		return messages.GetCardByIDResponse{
			SerialNumber: types.SerialNumber(controller),
			CardNumber:   record.Card,
			From:         types.Date(record.StartDate),
			To:           types.Date(record.EndDate),
			Door1:        record.Permissions[1],
			Door2:        record.Permissions[2],
			Door3:        record.Permissions[3],
			Door4:        record.Permissions[4],
			PIN:          types.PIN(record.PIN),
		}, nil
	}
}
