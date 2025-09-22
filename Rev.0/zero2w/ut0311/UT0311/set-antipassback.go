package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
	"ut0311/system"
)

var antipassbacks = map[uint8]system.AntiPassback{
	0x00: system.NoAntiPassback, // disabled
	0x01: system.Readers12_34,   // readers 1:2; 3:4 (independently)
	0x02: system.Readers13_24,   // readers (1,3):(2,4)
	0x03: system.Readers1_23,    // readers 1:(2,3)
	0x04: system.Readers1_234,   // readers 1:(2,3,4)
}

func (ut0311 *UT0311) setAntiPassback(rq *messages.SetAntiPassbackRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if antipassback, ok := antipassbacks[rq.AntiPassback]; !ok {
		return nil, fmt.Errorf("invalid anti-passback mode (%v)", rq.AntiPassback)
	} else if v, err := ut0311.system.SetAntiPassback(controller, antipassback); err != nil {
		return nil, err
	} else {
		return messages.SetAntiPassbackResponse{
			SerialNumber: types.SerialNumber(controller),
			Ok:           v == antipassback,
		}, nil
	}
}
