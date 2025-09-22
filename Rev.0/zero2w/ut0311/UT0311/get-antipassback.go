package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) getAntiPassback(rq *messages.GetAntiPassbackRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if antipassback, err := ut0311.system.GetAntiPassback(controller); err != nil {
		return nil, err
	} else {
		return messages.GetAntiPassbackResponse{
			SerialNumber: types.SerialNumber(controller),
			AntiPassback: uint8(antipassback),
		}, nil
	}
}
