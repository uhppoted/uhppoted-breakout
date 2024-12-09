package UT0311

import (
	"net/netip"

	"github.com/uhppoted/uhppote-core/messages"

	"emulator/MIB"
	"emulator/config"
)

func (ut0311 *UT0311) getListener(rq *messages.GetListenerRequest) (messages.GetListenerResponse, error) {
	response := messages.GetListenerResponse{}

	if listener, err := config.Get[netip.AddrPort](ut0311.config, MIB.OID_EVENTS_LISTENER); err != nil {
		return response, err
	} else {
		response.AddrPort = listener
	}

	if interval, err := config.Get[uint8](ut0311.config, MIB.OID_EVENTS_INTERVAL); err != nil {
		return response, err
	} else {
		response.Interval = interval
	}

	return response, nil
}
