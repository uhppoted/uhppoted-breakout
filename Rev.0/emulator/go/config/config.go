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
)

type Config struct {
	filepath string  `json:"-"`
	Driver   driver  `json:"driver"`
	Network  network `json:"network"`
	Events   events  `json:"events"`
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

type events struct {
	Listener netip.AddrPort `json:"listener"`
	Interval uint8          `json:"interval"`
}

func Load(filepath string) (Config, []byte, error) {
	config := Config{
		filepath: filepath,
	}

	if bytes, err := os.ReadFile(filepath); err != nil {
		return Config{}, nil, err
	} else if err := json.Unmarshal(bytes, &config); err != nil {
		return Config{}, nil, err
	} else {
		hash := sha256.Sum224(bytes)

		return config, hash[:], nil
	}
}

func Save(c Config) error {
	filepath := c.filepath

	if bytes, err := json.MarshalIndent(c, "", "    "); err != nil {
		return err
	} else if f, err := os.CreateTemp("", "uhppoted.*.config"); err != nil {
		return err
	} else {
		defer os.Remove(f.Name())

		if _, err := f.Write(bytes); err != nil {
			f.Close()
			return err
		} else if err := f.Close(); err != nil {
			return err
		} else if os.Rename(f.Name(), filepath); err != nil {
			return err
		}
	}

	return nil
}

func Get[T any](c Config, oid MIB.OID) (T, error) {
	var null T

	if MIB.OID.Equal(oid, MIB.OID_CONTROLLER_ADDRESS) {
		if v, ok := any(c.Network.IPv4.Address).(T); ok {
			return v, nil
		}
	}

	if MIB.OID.Equal(oid, MIB.OID_CONTROLLER_NETMASK) {
		if v, ok := any(c.Network.IPv4.Netmask).(T); ok {
			return v, nil
		}
	}

	if MIB.OID.Equal(oid, MIB.OID_CONTROLLER_GATEWAY) {
		if v, ok := any(c.Network.IPv4.Gateway).(T); ok {
			return v, nil
		}
	}

	if MIB.OID.Equal(oid, MIB.OID_CONTROLLER_MAC) {
		if v, ok := any(c.Network.IPv4.MAC).(T); ok {
			return v, nil
		}
	}

	if MIB.OID.Equal(oid, MIB.OID_EVENTS_LISTENER) {
		if v, ok := any(c.Events.Listener).(T); ok {
			return v, nil
		}
	}

	if MIB.OID.Equal(oid, MIB.OID_EVENTS_INTERVAL) {
		if v, ok := any(c.Events.Interval).(T); ok {
			return v, nil
		}
	}

	return null, fmt.Errorf("invalid OID (%v)", oid)
}

func Set(c *Config, oid MIB.OID, v any) error {
	if MIB.OID.Equal(oid, MIB.OID_EVENTS_LISTENER) {
		if addrPort, ok := v.(netip.AddrPort); !ok {
			return fmt.Errorf("invalid events listener address:port (%v)", v)
		} else {
			c.Events.Listener = addrPort

			return nil
		}
	}

	if MIB.OID.Equal(oid, MIB.OID_EVENTS_INTERVAL) {
		if interval, ok := v.(uint8); !ok {
			return fmt.Errorf("invalid events listener interval (%v)", v)
		} else {
			c.Events.Interval = interval

			return nil
		}
	}

	return fmt.Errorf("invalid OID (%v)", oid)
}

func (v *IPv4) MarshalJSON() ([]byte, error) {
	ipv4 := struct {
		Address netip.Addr `json:"address"`
		Netmask string     `json:"netmask"`
		Gateway netip.Addr `json:"gateway"`
		MAC     string     `json:"MAC"`
	}{
		Address: v.Address,
		Netmask: fmt.Sprintf("%d.%d.%d.%d", v.Netmask[0], v.Netmask[1], v.Netmask[2], v.Netmask[3]),
		Gateway: v.Gateway,
		MAC:     fmt.Sprintf("%v", v.MAC),
	}

	return json.Marshal(ipv4)
	// 	return err
	// } else if match := re.FindStringSubmatch(ipv4.Netmask); match == nil {
	// 	return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	// } else if a, err := strconv.ParseUint(match[1], 10, 8); err != nil {
	// 	return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	// } else if b, err := strconv.ParseUint(match[2], 10, 8); err != nil {
	// 	return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	// } else if c, err := strconv.ParseUint(match[3], 10, 8); err != nil {
	// 	return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	// } else if d, err := strconv.ParseUint(match[4], 10, 8); err != nil {
	// 	return fmt.Errorf("invalid netmask (%v)", ipv4.Netmask)
	// } else if MAC, err := net.ParseMAC(ipv4.MAC); err != nil {
	// 	return err
	// } else {
	// 	v.Address = ipv4.Address
	// 	v.Netmask = net.IPv4Mask(uint8(a), uint8(b), uint8(c), uint8(d))
	// 	v.Gateway = ipv4.Gateway
	// 	v.MAC = MAC

	// 	return nil
	// }
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
