package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) setTime(rq *messages.SetTimeRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if v, err := scmp.Set[types.DateTime](ut0311.driver, scmp.OID_CONTROLLER_DATETIME, rq.DateTime); err != nil {
		return nil, err
	} else {
		response := messages.SetTimeResponse{
			SerialNumber: types.SerialNumber(id),
			DateTime:     v,
		}

		return response, nil
	}
}
