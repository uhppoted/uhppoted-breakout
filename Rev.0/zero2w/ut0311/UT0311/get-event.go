package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) getEvent(rq *messages.GetEventRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if event, err := ut0311.db.Get(rq.Index); err != nil {
		return nil, err
	} else {
		response := messages.GetEventResponse{
			SerialNumber: types.SerialNumber(id),
			Index:        event.Index,
			Type:         uint8(event.Type),
			Granted:      event.Granted,
			Door:         event.Door,
			Direction:    event.Direction,
			CardNumber:   event.Card,
			Timestamp:    types.DateTime(event.Timestamp),
			Reason:       uint8(event.Reason),
		}

		return response, nil
	}
}
