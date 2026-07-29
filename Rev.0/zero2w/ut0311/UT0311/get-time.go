package UT0311

import (
	"codeberg.org/uhppoted/uhppoted-core/messages"
	"codeberg.org/uhppoted/uhppoted-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) getTime(rq *messages.GetTimeRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if datetime, err := scmp.Get[types.DateTime](ut0311.breakout, scmp.OID_CONTROLLER_DATETIME); err != nil {
		return nil, err
	} else {
		response := messages.GetTimeResponse{
			SerialNumber: types.SerialNumber(id),
			DateTime:     datetime,
		}

		return response, nil
	}
}
