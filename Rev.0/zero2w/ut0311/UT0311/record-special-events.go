package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) recordSpecialEvents(rq *messages.RecordSpecialEventsRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if _, err := scmp.Set(ut0311.events, scmp.OID_EVENTS_RECORD_ALL, rq.Enable); err != nil {
		return nil, err
	} else {
		return messages.RecordSpecialEventsResponse{
			SerialNumber: types.SerialNumber(id),
			Succeeded:    true,
		}, nil
	}
}
