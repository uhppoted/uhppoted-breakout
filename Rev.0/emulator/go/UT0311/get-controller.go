package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
)

func (ut0311 *UT0311) GetController(rq *messages.GetDeviceRequest) (messages.GetDeviceResponse, error) {
	response := messages.GetDeviceResponse{}

	if id, err := ut0311.driver.Get(MIB.OID_CONTROLLER_ID); err != nil {
		return response, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return response, fmt.Errorf("invalid controller ID (%v)", id)
	} else {
		response.SerialNumber = types.SerialNumber(id)
	}

	if address, err := ut0311.config.Get(MIB.OID_CONTROLLER_ADDRESS); err != nil {
		return response, err
	} else {
		response.IpAddress = address.AsSlice()
	}

	// netmask := MIB.Get(MIB.CONTROLLER_NETMASK, netip.IPv4Unspecified())
	// gateway := MIB.Get(MIB.CONTROLLER_GATEWAY, netip.IPv4Unspecified())
	// MAC := MIB.Get(MIB.CONTROLLER_MAC, net.HardwareAddr{})
	// version := MIB.Get(MIB.CONTROLLER_VERSION, uint16(0))
	// released := MIB.Get(MIB.CONTROLLER_RELEASED, "2000-01-01")

	// if id != 0 && (rq.SerialNumber == 0 || uint32(rq.SerialNumber) == id) {
	// response := messages.GetDeviceResponse{
	// 	SubnetMask:   netmask.AsSlice(),
	// 	Gateway:      gateway.AsSlice(),
	// 	MacAddress:   types.MacAddress(MAC),
	// 	Version:      types.Version(version),
	// 	Date:         types.MustParseDate(released),
	// }

	// }

	return response, nil
}
