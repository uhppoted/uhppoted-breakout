package UT0311

import (
	"encoding/json"
	"fmt"
	"net"
	"net/netip"
	"regexp"
	"strconv"
)

type Config struct {
	Network network `json:"network"`
}

type network struct {
	Interface string `json:"interface"`
	IPv4      *IPv4  `json:"IPv4"`
}

type IPv4 struct {
	Address netip.Addr       `json:"address"`
	Netmask net.IPMask       `json:"netmask"`
	Gateway netip.Addr       `json:"gateway"`
	MAC     net.HardwareAddr `json:"MAC"`
}

func (v *IPv4) UnmarshalJSON(bytes []byte) error {
	re := regexp.MustCompile(`([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)`)
	ipv4 := struct {
		Address netip.Addr `json:"address"`
		Netmask string     `json:"netmask"`
		Gateway netip.Addr `json:"gateway"`
		MAC     string     `json:"MAC"`
	}{}

	if err := json.Unmarshal(bytes, &ipv4); err != nil {
		return err
	} else if match := re.FindStringSubmatch(ipv4.Netmask); match == nil {
		return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	} else if a, err := strconv.ParseUint(match[1], 10, 8); err != nil {
		return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	} else if b, err := strconv.ParseUint(match[2], 10, 8); err != nil {
		return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	} else if c, err := strconv.ParseUint(match[3], 10, 8); err != nil {
		return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	} else if d, err := strconv.ParseUint(match[4], 10, 8); err != nil {
		return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	} else if MAC, err := net.ParseMAC(ipv4.MAC); err != nil {
		return err
	} else {
		v.Address = ipv4.Address
		v.Netmask = net.IPv4Mask(uint8(a), uint8(b), uint8(c), uint8(d))
		v.Gateway = ipv4.Gateway
		v.MAC = MAC

		return nil
	}
}
