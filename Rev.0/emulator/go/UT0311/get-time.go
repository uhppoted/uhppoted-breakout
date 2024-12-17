package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
	"emulator/driver"
)

func (ut0311 *UT0311) getTime(rq *messages.GetTimeRequest) (any, error) {
	if id, err := driver.Get[uint32](ut0311.driver, MIB.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if datetime, err := driver.Get[types.DateTime](ut0311.driver, MIB.OID_CONTROLLER_DATETIME); err != nil {
		return nil, err
	} else {
		response := messages.GetTimeResponse{
			SerialNumber: types.SerialNumber(id),
			DateTime:     datetime,
		}

		return response, nil
	}
}
