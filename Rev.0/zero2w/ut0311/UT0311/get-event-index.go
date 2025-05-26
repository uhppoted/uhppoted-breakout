package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) getEventIndex(rq *messages.GetEventIndexRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetEventIndexResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if index, err := scmp.Get[uint32](ut0311.events, scmp.OID_EVENTS_INDEX); err != nil {
			return nil, err
		} else {
			response.Index = index
		}

		return response, nil
	}
}
