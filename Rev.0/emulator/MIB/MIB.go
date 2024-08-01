package MIB

import (
	"fmt"
	"net"
	"net/netip"
)

type V interface {
	uint32 | uint16 | string | netip.Addr | net.HardwareAddr
}

const CONTROLLER_ID = "controller.id"
const CONTROLLER_ADDRESS = "controller.address"
const CONTROLLER_NETMASK = "controller.netmask"
const CONTROLLER_GATEWAY = "controller.gateway"
const CONTROLLER_MAC = "controller.MAC"
const CONTROLLER_VERSION = "controller.version"
const CONTROLLER_RELEASED = "controller.released"

var mib = map[string]any{
	CONTROLLER_ID:       uint32(405419896),
	CONTROLLER_ADDRESS:  netip.AddrFrom4([4]byte{192, 168, 1, 100}),
	CONTROLLER_NETMASK:  netip.AddrFrom4([4]byte{255, 255, 255, 0}),
	CONTROLLER_GATEWAY:  netip.AddrFrom4([4]byte{192, 168, 1, 1}),
	CONTROLLER_MAC:      MustParseMAC("00:66:19:39:55:2d"),
	CONTROLLER_VERSION:  uint16(0x2407),
	CONTROLLER_RELEASED: "2024-07-30",
}

func Get[T V](tag string, defval T) T {
	if u, ok := mib[tag]; ok {
		if v, ok := u.(T); ok {
			return v
		}
	}

	return defval
}

func MustParseMAC(s string) net.HardwareAddr {
	if mac, err := net.ParseMAC(s); err != nil {
		panic(fmt.Sprintf("invalid MAC address '%v'", s))
	} else {
		return mac
	}
}
