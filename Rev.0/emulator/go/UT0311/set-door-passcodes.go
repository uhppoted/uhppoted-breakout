package UT0311

import (
	"slices"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) setDoorPasscodes(rq *messages.SetDoorPasscodesRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if door := rq.Door; door < 1 || door > 4 {
		return nil, nil
	} else {
		passcodes := []uint32{
			rq.Passcode1,
			rq.Passcode2,
			rq.Passcode3,
			rq.Passcode4,
		}

		response := messages.SetDoorPasscodesResponse{
			SerialNumber: types.SerialNumber(id),
			Succeeded:    false,
		}

		if oid, ok := lookup.passcodes[door]; !ok {
			return nil, nil
		} else if v, err := scmp.Set[[]uint32](ut0311.driver, oid, passcodes); err != nil {
			return nil, err
		} else {
			response.Succeeded = slices.Equal(v, passcodes)
		}

		return response, nil
	}
}
