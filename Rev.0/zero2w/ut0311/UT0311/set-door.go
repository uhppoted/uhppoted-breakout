package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/rpcd"
	"ut0311/scmp"
)

var modes = map[uint8]rpcd.DoorMode{
	1: rpcd.NormallyOpen,
	2: rpcd.NormallyClosed,
	3: rpcd.Controlled,
}

func (ut0311 *UT0311) setDoor(rq *messages.SetDoorControlStateRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if door := rq.Door; door < 1 || door > 4 {
		return nil, nil
	} else if m, ok := modes[rq.ControlState]; !ok {
		return nil, fmt.Errorf("invalid door control mode (%v)", rq.ControlState)
	} else if mode, err := ut0311.breakout.SetDoorMode(controller, door, m); err != nil {
		return nil, err
	} else if delay, err := ut0311.breakout.SetDoorDelay(controller, door, rq.Delay); err != nil {
		return nil, err
	} else {
		tag1 := fmt.Sprintf("controller.door.%v.mode", door)
		tag2 := fmt.Sprintf("controller.door.%v.delay", door)

		ut0311.state.Set(controller, tag1, mode)
		ut0311.state.Set(controller, tag2, delay)

		return messages.SetDoorControlStateResponse{
			SerialNumber: types.SerialNumber(controller),
			Door:         door,
			ControlState: mode,
			Delay:        delay,
		}, nil
	}
}
