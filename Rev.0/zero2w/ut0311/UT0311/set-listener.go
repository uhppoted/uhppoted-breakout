package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/config"
	"emulator/scmp"
)

func (ut0311 *UT0311) setListener(rq *messages.SetListenerRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else if _, err := scmp.Set(ut0311.config, scmp.OID_CONTROLLER_EVENT_LISTENER, rq.AddrPort); err != nil {
		return nil, err
	} else if _, err := scmp.Set(ut0311.config, scmp.OID_CONTROLLER_UPLOAD_INTERVAL, rq.Interval); err != nil {
		return nil, err
	} else if err := config.Save(ut0311.config); err != nil {
		return nil, err
	} else {
		return messages.SetListenerResponse{
			SerialNumber: types.SerialNumber(id),
			Succeeded:    true,
		}, nil
	}
}
