package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
)

func (ut0311 *UT0311) getTime(rq *messages.GetTimeRequest) (any, error) {
	if id, err := ut0311.getID(); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {

		if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_DATETIME); err != nil {
			return nil, err
		} else if s, ok := v.(string); !ok {
			return nil, fmt.Errorf("invalid controller date/timme (%v)", v)
		} else if datetime, err := types.ParseDateTime(s); err != nil {
			return nil, err
		} else {
			response := messages.GetTimeResponse{
				SerialNumber: types.SerialNumber(id),
				DateTime:     datetime,
			}

			return response, nil
		}
	}
}
