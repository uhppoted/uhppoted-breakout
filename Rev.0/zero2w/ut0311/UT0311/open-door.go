package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) openDoor(rq *messages.OpenDoorRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if door := rq.Door; door < 1 || door > 4 {
		return nil, nil
	} else if ok, err := ut0311.breakout.UnlockDoor(door); err != nil {
		return nil, err
	} else {
		return messages.OpenDoorResponse{
			SerialNumber: types.SerialNumber(id),
			Succeeded:    ok,
		}, nil
	}
}
