package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) getDoor(rq *messages.GetDoorControlStateRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if door := rq.Door; door < 1 || door > 4 {
		return nil, nil
	} else {
		response := messages.GetDoorControlStateResponse{
			SerialNumber: types.SerialNumber(id),
			Door:         door,
		}

		if oid, ok := lookup.modes[door]; !ok {
			return nil, nil
		} else if mode, err := scmp.Get[uint8](ut0311.driver, oid); err != nil {
			return nil, err
		} else {
			response.ControlState = mode
		}

		if oid, ok := lookup.delays[door]; !ok {
			return nil, nil
		} else if delay, err := scmp.Get[uint8](ut0311.driver, oid); err != nil {
			return nil, err
		} else {
			response.Delay = delay
		}

		return response, nil
	}
}