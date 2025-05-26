package UT0311

import (
	"net"
	"net/netip"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) getController(rq *messages.GetDeviceRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetDeviceResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if address, err := scmp.Get[netip.Addr](ut0311.config, scmp.OID_CONTROLLER_ADDRESS); err != nil {
			return nil, err
		} else {
			response.IpAddress = address.AsSlice()
		}

		if netmask, err := scmp.Get[net.IPMask](ut0311.config, scmp.OID_CONTROLLER_NETMASK); err != nil {
			return nil, err
		} else {
			response.SubnetMask = []byte(netmask)
		}

		if gateway, err := scmp.Get[netip.Addr](ut0311.config, scmp.OID_CONTROLLER_GATEWAY); err != nil {
			return nil, err
		} else {
			response.Gateway = gateway.AsSlice()
		}

		if MAC, err := scmp.Get[net.HardwareAddr](ut0311.config, scmp.OID_CONTROLLER_MAC); err != nil {
			return nil, err
		} else {
			response.MacAddress = []byte(MAC)
		}

		if version, err := scmp.Get[uint16](ut0311.breakout, scmp.OID_CONTROLLER_VERSION); err != nil {
			return nil, err
		} else {
			response.Version = types.Version(version)
		}

		if released, err := scmp.Get[types.Date](ut0311.breakout, scmp.OID_CONTROLLER_RELEASED); err != nil {
			return nil, err
		} else {
			response.Date = released
		}

		return response, nil
	}
}
