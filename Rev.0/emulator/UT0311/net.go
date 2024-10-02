package UT0311

import (
	"net"
	"net/netip"
)

// Ref. https://stackoverflow.com/questions/23529663/how-to-get-all-addresses-and-masks-from-local-interfaces-in-go
func resolveNetAddr(name string) (netip.Addr, netip.Addr, netip.Addr, error) {
	address := netip.AddrFrom4([4]byte{0, 0, 0, 0})
	netmask := netip.AddrFrom4([4]byte{255, 255, 255, 0})
	gateway := netip.AddrFrom4([4]byte{0, 0, 0, 0})

	if iface, err := net.InterfaceByName(name); err != nil {
		return address, netmask, gateway, err
	} else if addresses, err := iface.Addrs(); err != nil {
		return address, netmask, gateway, err
	} else {
		for _, addr := range addresses {
			switch v := addr.(type) {
			case *net.IPNet:
				if v.IP.To4() != nil && iface.Flags&net.FlagLoopback == 0 {
					ipv4 := [4]byte(v.IP.To4())
					mask := v.Mask

					address = netip.AddrFrom4(ipv4)
					netmask = netip.AddrFrom4([4]byte{mask[0], mask[1], mask[2], mask[3]})
					gateway = netip.AddrFrom4([4]byte{ipv4[0], ipv4[1], 0, 1})

					break
				}

			case *net.IPAddr:
				if v.IP.To4() != nil && iface.Flags&net.FlagLoopback == 0 {
					ipv4 := [4]byte(v.IP.To4())
					mask := v.IP.DefaultMask()

					address = netip.AddrFrom4(ipv4)
					netmask = netip.AddrFrom4([4]byte{mask[0], mask[1], mask[2], mask[3]})
					gateway = netip.AddrFrom4([4]byte{ipv4[0], ipv4[1], 0, 1})

					break
				}
			}
		}
	}

	return address, netmask, gateway, nil
}
