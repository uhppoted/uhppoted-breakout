package MIB

import (
	"fmt"
	"net"
	"net/netip"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/ssmp"
)

var controller = struct {
	id       uint32
	address  netip.Addr
	netmask  net.IPMask
	gateway  netip.Addr
	MAC      net.HardwareAddr
	version  uint16
	released string
}{
	id:       405419896,
	address:  netip.AddrFrom4([4]byte{0, 0, 0, 0}),
	netmask:  net.IPv4Mask(255, 255, 255, 0),
	gateway:  netip.AddrFrom4([4]byte{0, 0, 0, 0}),
	MAC:      net.HardwareAddr([]byte{0x00, 0x66, 0x19, 0x39, 0x55, 0x2d}),
	version:  0x0000,
	released: "2000-01-01",
}

func getControllerID() (any, error) {
	if oid, ok := OIDs[CONTROLLER_ID]; !ok {
		return nil, fmt.Errorf("missing %v OID", CONTROLLER_ID)
	} else if v, err := ssmp.Get(oid); err != nil {
		return nil, err
	} else if id, ok := v.(uint32); !ok {
		return nil, fmt.Errorf("invalid %v value", CONTROLLER_ID)
	} else {
		return id, nil
	}
}

func getControllerAddress() (any, error) {
	return controller.address, nil
}

func getControllerNetmask() (any, error) {
	return controller.netmask, nil
}

func getControllerGateway() (any, error) {
	return controller.gateway, nil
}

func getControllerMAC() (any, error) {
	return controller.MAC, nil
}

func getControllerVersion() (any, error) {
	if oid, ok := OIDs[CONTROLLER_VERSION]; !ok {
		return nil, fmt.Errorf("missing %v OID", CONTROLLER_VERSION)
	} else if v, err := ssmp.Get(oid); err != nil {
		return nil, err
	} else if bytes, ok := v.([]byte); !ok {
		return nil, fmt.Errorf("invalid %v value (%T)", CONTROLLER_VERSION, v)
	} else if len(bytes) != 2 {
		return nil, fmt.Errorf("invalid %v value (%v)", CONTROLLER_VERSION, bytes)
	} else {
		id := (uint16(bytes[0]) & 0x00ff) << 8
		id |= (uint16(bytes[1]) & 0x00ff) << 0

		return id, nil
	}
}

func getControllerReleased() (any, error) {
	if oid, ok := OIDs[CONTROLLER_RELEASED]; !ok {
		return nil, fmt.Errorf("missing %v OID", CONTROLLER_RELEASED)
	} else if v, err := ssmp.Get(oid); err != nil {
		return nil, err
	} else if bytes, ok := v.([]byte); !ok {
		return nil, fmt.Errorf("invalid %v value (%T)", CONTROLLER_RELEASED, v)
	} else {
		return string(bytes), nil
	}
}
