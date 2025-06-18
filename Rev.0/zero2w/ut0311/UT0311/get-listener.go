package UT0311

import (
	"net/netip"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"ut0311/scmp"
)

func (ut0311 *UT0311) getListener(rq *messages.GetListenerRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetListenerResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if listener, err := scmp.Get[netip.AddrPort](ut0311.config, scmp.OID_CONTROLLER_EVENT_LISTENER); err != nil {
			return nil, err
		} else {
			response.AddrPort = listener
		}

		if interval, err := scmp.Get[uint8](ut0311.config, scmp.OID_CONTROLLER_UPLOAD_INTERVAL); err != nil {
			return nil, err
		} else {
			response.Interval = interval
		}

		return response, nil
	}
}
