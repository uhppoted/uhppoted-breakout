package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) openDoor(rq *messages.OpenDoorRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if door := rq.Door; door < 1 || door > 4 {
		return nil, nil
	} else {
		response := messages.OpenDoorResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if oid, ok := lookup.lock[door]; !ok {
			return nil, nil
		} else if locked, err := scmp.Set[bool](ut0311.driver, oid, false); err != nil {
			return nil, err
		} else {
			response.Succeeded = locked == false
		}

		return response, nil
	}
}
