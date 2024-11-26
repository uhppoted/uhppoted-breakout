package config

import (
	"crypto/sha256"
	"encoding/json"
	"fmt"
	"net"
	"net/netip"
	"os"
	"regexp"
	"strconv"

	"emulator/MIB"
	"emulator/MIB/types"
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

func Load(filepath string) (Config, []byte, error) {
	var config Config

	if bytes, err := os.ReadFile(filepath); err != nil {
		return Config{}, nil, err
	} else if err := json.Unmarshal(bytes, &config); err != nil {
		return Config{}, nil, err
	} else {
		hash := sha256.Sum224(bytes)

		return config, hash[:], nil
	}
}

func (c Config) Get(oid types.OID) (netip.Addr, error) {
	if types.OID.Equal(oid, MIB.OID_CONTROLLER_ADDRESS) {
		return c.Network.IPv4.Address, nil
	}

	return netip.Addr{}, fmt.Errorf("invalid OID (%v)", oid)
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
