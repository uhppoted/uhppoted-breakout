package UT0311

import (
	"fmt"
	"net"
	"net/netip"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
	"emulator/config"
)

func (ut0311 *UT0311) getController(rq *messages.GetDeviceRequest) (any, error) {
	if id, err := ut0311.getID(); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetDeviceResponse{
			SerialNumber: types.SerialNumber(id),
		}

		if address, err := config.Get[netip.Addr](ut0311.config, MIB.OID_CONTROLLER_ADDRESS); err != nil {
			return nil, err
		} else {
			response.IpAddress = address.AsSlice()
		}

		if netmask, err := config.Get[net.IPMask](ut0311.config, MIB.OID_CONTROLLER_NETMASK); err != nil {
			return nil, err
		} else {
			response.SubnetMask = []byte(netmask)
		}

		if gateway, err := config.Get[netip.Addr](ut0311.config, MIB.OID_CONTROLLER_GATEWAY); err != nil {
			return nil, err
		} else {
			response.Gateway = gateway.AsSlice()
		}

		if MAC, err := config.Get[net.HardwareAddr](ut0311.config, MIB.OID_CONTROLLER_MAC); err != nil {
			return nil, err
		} else {
			response.MacAddress = []byte(MAC)
		}

		if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_VERSION); err != nil {
			return nil, err
		} else if version, ok := v.(uint16); !ok {
			return nil, fmt.Errorf("invalid controller version (%v)", version)
		} else {
			response.Version = types.Version(version)
		}

		if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_RELEASED); err != nil {
			return nil, err
		} else if released, ok := v.(string); !ok {
			return nil, fmt.Errorf("invalid controller release date (%v)", released)
		} else if date, err := types.ParseDate(released); err != nil {
			return nil, fmt.Errorf("invalid controller release date (%v)", released)
		} else {
			response.Date = date
		}

		return response, nil
	}
}
