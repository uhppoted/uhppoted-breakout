package MIB

import (
	"net"
	"net/netip"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/log"
)

type V interface {
	uint32 | uint16 | string | netip.Addr | net.HardwareAddr
}

type field struct {
	get func() (any, error)
}

const CONTROLLER_ID = "controller.id"
const CONTROLLER_ADDRESS = "controller.address"
const CONTROLLER_NETMASK = "controller.netmask"
const CONTROLLER_GATEWAY = "controller.gateway"
const CONTROLLER_MAC = "controller.MAC"
const CONTROLLER_VERSION = "controller.version"
const CONTROLLER_RELEASED = "controller.released"

var mib = map[string]field{
	CONTROLLER_ID:       {get: getControllerID},
	CONTROLLER_ADDRESS:  {get: getControllerAddress},
	CONTROLLER_NETMASK:  {get: getControllerNetmask},
	CONTROLLER_GATEWAY:  {get: getControllerGateway},
	CONTROLLER_MAC:      {get: getControllerMAC},
	CONTROLLER_VERSION:  {get: getControllerVersion},
	CONTROLLER_RELEASED: {get: getControllerReleased},
}

func Get[T V](tag string, defval T) T {
	if f, ok := mib[tag]; ok && f.get != nil {
		if u, err := f.get(); err != nil {
			warnf("GET %v (%v)", tag, err)
		} else if v, ok := u.(T); ok {
			return v
		}
	}

	return defval
}

func warnf(format string, args ...any) {
	log.Warnf("MIB", format, args...)
}
