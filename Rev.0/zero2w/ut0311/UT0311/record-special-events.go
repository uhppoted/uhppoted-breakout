package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	lib "github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) recordSpecialEvents(rq *messages.RecordSpecialEventsRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if err := ut0311.events.RecordSpecialEvents(controller, rq.Enable); err != nil {
		return nil, err
	} else {
		return messages.RecordSpecialEventsResponse{
			SerialNumber: lib.SerialNumber(controller),
			Succeeded:    true,
		}, nil
	}
}
