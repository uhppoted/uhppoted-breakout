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
	Driver  driver  `json:"driver"`
	Network network `json:"network"`
}

type driver struct {
	Driver string `json:"driver"`
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

func Get[T any](c Config, oid types.OID) (T, error) {
	var null T

	if types.OID.Equal(oid, MIB.OID_CONTROLLER_ADDRESS) {
		if v, ok := any(c.Network.IPv4.Address).(T); ok {
			return v, nil
		}
	}

	if types.OID.Equal(oid, MIB.OID_CONTROLLER_NETMASK) {
		if v, ok := any(c.Network.IPv4.Netmask).(T); ok {
			return v, nil
		}
	}

	if types.OID.Equal(oid, MIB.OID_CONTROLLER_GATEWAY) {
		if v, ok := any(c.Network.IPv4.Gateway).(T); ok {
			return v, nil
		}
	}

	if types.OID.Equal(oid, MIB.OID_CONTROLLER_MAC) {
		if v, ok := any(c.Network.IPv4.MAC).(T); ok {
			return v, nil
		}
	}

	return null, fmt.Errorf("invalid OID (%v)", oid)
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
