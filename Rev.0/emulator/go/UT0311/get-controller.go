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

func (ut0311 *UT0311) getController(rq *messages.GetDeviceRequest) (messages.GetDeviceResponse, error) {
	response := messages.GetDeviceResponse{}

	if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_ID); err != nil {
		return response, err
	} else if id, ok := v.(uint32); !ok {
		return response, fmt.Errorf("invalid controller ID (%v)", id)
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return response, fmt.Errorf("invalid controller ID (%v)", id)
	} else {
		response.SerialNumber = types.SerialNumber(id)
	}

	if address, err := config.Get[netip.Addr](ut0311.config, MIB.OID_CONTROLLER_ADDRESS); err != nil {
		return response, err
	} else {
		response.IpAddress = address.AsSlice()
	}

	if netmask, err := config.Get[net.IPMask](ut0311.config, MIB.OID_CONTROLLER_NETMASK); err != nil {
		return response, err
	} else {
		response.SubnetMask = []byte(netmask)
	}

	if gateway, err := config.Get[netip.Addr](ut0311.config, MIB.OID_CONTROLLER_GATEWAY); err != nil {
		return response, err
	} else {
		response.Gateway = gateway.AsSlice()
	}

	if MAC, err := config.Get[net.HardwareAddr](ut0311.config, MIB.OID_CONTROLLER_MAC); err != nil {
		return response, err
	} else {
		response.MacAddress = []byte(MAC)
	}

	if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_VERSION); err != nil {
		return response, err
	} else if version, ok := v.(uint16); !ok {
		return response, fmt.Errorf("invalid controller version (%v)", version)
	} else {
		response.Version = types.Version(version)
	}

	if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_RELEASED); err != nil {
		return response, err
	} else if released, ok := v.(string); !ok {
		return response, fmt.Errorf("invalid controller release date (%v)", released)
	} else if date, err := types.ParseDate(released); err != nil {
		return response, fmt.Errorf("invalid controller release date (%v)", released)
	} else {
		response.Date = date
	}

	return response, nil
}
