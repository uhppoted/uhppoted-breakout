package UT0311

import (
	"net/netip"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
	"emulator/config"
	"emulator/driver"
)

func (ut0311 *UT0311) getListener(rq *messages.GetListenerRequest) (any, error) {
	if id, err := driver.Get[uint32](ut0311.driver, MIB.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetListenerResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if listener, err := config.Get[netip.AddrPort](ut0311.config, MIB.OID_EVENTS_LISTENER); err != nil {
			return nil, err
		} else {
			response.AddrPort = listener
		}

		if interval, err := config.Get[uint8](ut0311.config, MIB.OID_EVENTS_INTERVAL); err != nil {
			return nil, err
		} else {
			response.Interval = interval
		}

		return response, nil
	}
}
