package UT0311

import (
	"codeberg.org/uhppoted/uhppoted-core/messages"
	"codeberg.org/uhppoted/uhppoted-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) setEventIndex(rq *messages.SetEventIndexRequest) (any, error) {
	if controller, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if controller == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != controller) {
		return nil, nil
	} else if current, err := ut0311.events.GetEventIndex(controller); err != nil {
		return nil, err
	} else if current == rq.Index {
		response := messages.SetEventIndexResponse{
			SerialNumber: types.SerialNumber(controller),
			Changed:      false,
		}

		return response, nil
	} else if err := ut0311.events.SetEventIndex(controller, rq.Index); err != nil {
		return nil, err
	} else {
		response := messages.SetEventIndexResponse{
			SerialNumber: types.SerialNumber(controller),
			Changed:      true,
		}

		return response, nil
	}
}
