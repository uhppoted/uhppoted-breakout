package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
	"emulator/config"
)

func (ut0311 *UT0311) setListener(rq *messages.SetListenerRequest) (any, error) {
	if id, err := ut0311.getID(); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.SetListenerResponse{
			SerialNumber: types.SerialNumber(id),
			Succeeded:    false,
		}

		if err := config.Set(&ut0311.config, MIB.OID_EVENTS_LISTENER, rq.AddrPort); err != nil {
			return response, err
		} else if err := config.Set(&ut0311.config, MIB.OID_EVENTS_INTERVAL, rq.Interval); err != nil {
			return response, err
		} else if err := config.Save(ut0311.config); err != nil {
			return response, err
		}

		response.Succeeded = true

		return response, nil
	}
}