package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
)

func (ut0311 *UT0311) setTime(rq *messages.SetTimeRequest) (any, error) {
	if id, err := ut0311.getID(); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {

		datetime := fmt.Sprintf("%v", rq.DateTime)

		if v, err := ut0311.driver.Set(MIB.OID_CONTROLLER_DATETIME, datetime); err != nil {
			return nil, err
		} else if s, ok := v.(string); !ok {
			return nil, fmt.Errorf("invalid controller date/time (%v)", v)
		} else if dt, err := types.ParseDateTime(s); err != nil {
			return nil, err
		} else {
			response := messages.SetTimeResponse{
				SerialNumber: types.SerialNumber(id),
				DateTime:     dt,
			}

			return response, nil
		}
	}
}
