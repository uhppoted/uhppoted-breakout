package UT0311

import (
	"net"
	"net/netip"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
)

func (ut0311 *UT0311) GetController(rq *messages.GetDeviceRequest) (*messages.GetDeviceResponse, error) {
	id := MIB.Get(MIB.CONTROLLER_ID, uint32(0))
	address := MIB.Get(MIB.CONTROLLER_ADDRESS, netip.IPv4Unspecified())
	netmask := MIB.Get(MIB.CONTROLLER_NETMASK, netip.IPv4Unspecified())
	gateway := MIB.Get(MIB.CONTROLLER_GATEWAY, netip.IPv4Unspecified())
	MAC := MIB.Get(MIB.CONTROLLER_MAC, net.HardwareAddr{})
	version := MIB.Get(MIB.CONTROLLER_VERSION, uint16(0))
	released := MIB.Get(MIB.CONTROLLER_RELEASED, "2000-01-01")

	if id != 0 && (rq.SerialNumber == 0 || uint32(rq.SerialNumber) == id) {
		response := messages.GetDeviceResponse{
			SerialNumber: types.SerialNumber(id),
			IpAddress:    address.AsSlice(),
			SubnetMask:   netmask.AsSlice(),
			Gateway:      gateway.AsSlice(),
			MacAddress:   types.MacAddress(MAC),
			Version:      types.Version(version),
			Date:         types.MustParseDate(released),
		}

		return &response, nil
	}

	return nil, nil
}
