package UT0311

import (
	"net"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"
)

func (ut0311 *UT0311) GetController(rq *messages.GetDeviceRequest) (*messages.GetDeviceResponse, error) {
	if rq.SerialNumber != 0 && rq.SerialNumber != 405419896 {
		return nil, nil
	}

	MAC, _ := net.ParseMAC("00:66:19:39:55:2d")

	response := messages.GetDeviceResponse{
		SerialNumber: 405419896,
		IpAddress:    net.IPv4(192, 168, 1, 100),
		SubnetMask:   net.IPv4(255, 255, 255, 0),
		Gateway:      net.IPv4(192, 168, 1, 1),
		MacAddress:   types.MacAddress(MAC),
		Version:      0x2407,
		Date:         types.MustParseDate("2024-07-30"),
	}

	return &response, nil
}
