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
	netmask  netip.Addr
	gateway  netip.Addr
	MAC      net.HardwareAddr
	version  uint16
	released string
}{
	id:       405419896,
	address:  netip.AddrFrom4([4]byte{192, 168, 1, 100}),
	netmask:  netip.AddrFrom4([4]byte{255, 255, 255, 0}),
	gateway:  netip.AddrFrom4([4]byte{192, 168, 1, 1}),
	MAC:      net.HardwareAddr([]byte{0x00, 0x66, 0x19, 0x39, 0x55, 0x2d}),
	version:  0x2407,
	released: "2024-07-30",
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
	return controller.version, nil
}

func getControllerReleased() (any, error) {
	return controller.released, nil
}
