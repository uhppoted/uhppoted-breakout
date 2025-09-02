package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/rpcd"
	"ut0311/scmp"
)

var interlocks = map[uint8]rpcd.Interlock{
	0x00: rpcd.NoInterlock,    // disabled
	0x01: rpcd.Interlock12,    // doors (1,2)
	0x02: rpcd.Interlock34,    // doors (3,4)
	0x03: rpcd.Interlock12_34, // doors (1,2) and (3,4)
	0x04: rpcd.Interlock123,   // doors (1,2,3)
	0x08: rpcd.Interlock1234,  // doors (1,2,3,4)
}

func (ut0311 *UT0311) setInterlock(rq *messages.SetInterlockRequest) (any, error) {
	tag := "controller.doors.interlock"

	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if interlock, ok := interlocks[rq.Interlock]; !ok {
		return nil, fmt.Errorf("invalid interlock (%v)", rq.Interlock)
	} else if v, err := ut0311.breakout.SetInterlock(controller, interlock); err != nil {
		return nil, err
	} else if v != interlock {
		ut0311.state.Set(controller, tag, interlock)

		return messages.SetInterlockResponse{
			SerialNumber: types.SerialNumber(controller),
			Succeeded:    false,
		}, nil
	} else {

		ut0311.state.Set(controller, tag, interlock)

		return messages.SetInterlockResponse{
			SerialNumber: types.SerialNumber(controller),
			Succeeded:    true,
		}, nil
	}
}
