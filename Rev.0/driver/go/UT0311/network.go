package UT0311

import (
	"fmt"
	"net"
	"net/netip"

	jackpal "github.com/jackpal/gateway"
)

// Ref. https://stackoverflow.com/questions/23529663/how-to-get-all-addresses-and-masks-from-local-interfaces-in-go
func discover(name string) (netip.Addr, net.IPMask, netip.Addr, net.HardwareAddr, error) {
	var address netip.Addr
	var netmask net.IPMask
	var gateway netip.Addr
	var MAC net.HardwareAddr

	if iface, err := net.InterfaceByName(name); err != nil {
		return address, netmask, gateway, MAC, err
	} else if addresses, err := iface.Addrs(); err != nil {
		return address, netmask, gateway, MAC, err
	} else {
		for _, addr := range addresses {
			switch v := addr.(type) {
			case *net.IPNet:
				if v.IP.To4() != nil && iface.Flags&net.FlagLoopback == 0 {
					ipv4 := [4]byte(v.IP.To4())
					mask := v.Mask

					address = netip.AddrFrom4(ipv4)
					netmask = net.IPv4Mask(mask[0], mask[1], mask[2], mask[3])
					MAC = iface.HardwareAddr

					break
				}

			case *net.IPAddr:
				if v.IP.To4() != nil && iface.Flags&net.FlagLoopback == 0 {
					ipv4 := [4]byte(v.IP.To4())
					mask := v.IP.DefaultMask()

					address = netip.AddrFrom4(ipv4)
					netmask = net.IPv4Mask(mask[0], mask[1], mask[2], mask[3])
					MAC = iface.HardwareAddr

					break
				}
			}
		}
	}

	if v, err := jackpal.DiscoverGateway(); err != nil {
		return address, netmask, gateway, MAC, err
	} else if addr, ok := netip.AddrFromSlice(v); !ok {
		return address, netmask, gateway, MAC, fmt.Errorf("invalid gateway (%v)", v)
	} else {
		gateway = netip.AddrFrom4(addr.As4())
	}

	return address, netmask, gateway, MAC, nil
}
