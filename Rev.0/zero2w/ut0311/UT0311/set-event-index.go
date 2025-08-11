package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) setEventIndex(rq *messages.SetEventIndexRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if current, err := ut0311.db.GetEventIndex(controller); err != nil {
		return nil, err
	} else if current == rq.Index {
		response := messages.SetEventIndexResponse{
			SerialNumber: types.SerialNumber(controller),
			Changed:      false,
		}

		return response, nil
	} else if err := ut0311.db.SetEventIndex(controller, rq.Index); err != nil {
		return nil, err
	} else {
		response := messages.SetEventIndexResponse{
			SerialNumber: types.SerialNumber(controller),
			Changed:      true,
		}

		return response, nil
	}
}
