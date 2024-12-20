package MIB

import (
	"fmt"
	"net"
	"net/netip"

	"emulator/MIB/types"
	"emulator/log"
)

type V interface {
	uint32 | uint16 | string | netip.Addr | net.HardwareAddr
}

type field struct {
	get func() (any, error)
}

const SYSTEM_SERIALNO = "system.serialno"

const CONTROLLER_ID = "controller.id"
const CONTROLLER_VERSION = "controller.version"
const CONTROLLER_RELEASED = "controller.released"
const CONTROLLER_ADDRESS = "controller.address"
const CONTROLLER_NETMASK = "controller.netmask"
const CONTROLLER_GATEWAY = "controller.gateway"
const CONTROLLER_MAC = "controller.MAC"

var OIDs = map[string]types.OID{
	SYSTEM_SERIALNO:     []uint32{1, 3, 6, 1, 4, 1, 65536, 1, 1},
	CONTROLLER_ID:       []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 1},
	CONTROLLER_VERSION:  []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 2},
	CONTROLLER_RELEASED: []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 3},
	CONTROLLER_ADDRESS:  []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 4},
	CONTROLLER_NETMASK:  []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 5},
	CONTROLLER_GATEWAY:  []uint32{1, 3, 6, 1, 4, 1, 65536, 2, 6},
}

var mib = map[string]field{
	SYSTEM_SERIALNO: {get: getSystemSerialNo},

	CONTROLLER_ID:       {get: getControllerID},
	CONTROLLER_ADDRESS:  {get: getControllerAddress},
	CONTROLLER_NETMASK:  {get: getControllerNetmask},
	CONTROLLER_GATEWAY:  {get: getControllerGateway},
	CONTROLLER_MAC:      {get: getControllerMAC},
	CONTROLLER_VERSION:  {get: getControllerVersion},
	CONTROLLER_RELEASED: {get: getControllerReleased},
}

func Init(address netip.Addr, netmask net.IPMask, gateway netip.Addr, MAC net.HardwareAddr) error {
	if !address.IsValid() {
		return fmt.Errorf("invalid controller IPv4 address (%v)", address)
	}

	if netmask == nil {
		return fmt.Errorf("invalid controller IPv4 netmask (%v)", netmask)
	}

	if !gateway.IsValid() {
		return fmt.Errorf("invalid controller IPv4 gateway (%v)", gateway)
	}

	if len(MAC) != 6 {
		return fmt.Errorf("invalid controller MAC address (%v)", MAC)
	}

	controller.address = address
	controller.netmask = netmask
	controller.gateway = gateway
	controller.MAC = MAC

	return nil
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
