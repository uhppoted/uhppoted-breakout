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

	"emulator/scmp"
)

type Config struct {
	filepath string  `json:"-"`
	Driver   driver  `json:"driver"`
	Network  network `json:"network"`
	Events   events  `json:"events"`
	TLS      TLS     `json:"TLS"`
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

type TLS struct {
	Certificate string `json:"certificate"`
	CA          string `json:"CA"`
}

type events struct {
	Listener netip.AddrPort `json:"listener"`
	Interval uint8          `json:"interval"`
}

func Load(filepath string) (Config, []byte, error) {
	config := Config{
		filepath: filepath,
		TLS: TLS{
			Certificate: ".certificate",
			CA:          ".CA",
		},
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

func (c Config) GetUint8(oid scmp.OID) (uint8, error) {
	if scmp.Is(oid, scmp.OID_CONTROLLER_UPLOAD_INTERVAL) {
		return c.Events.Interval, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Config) GetUint16(oid scmp.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Config) GetUint32(oid scmp.OID) (uint32, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Config) GetBool(oid scmp.OID) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (c Config) GetString(oid scmp.OID) (string, error) {
	if scmp.Is(oid, scmp.OID_CONTROLLER_EVENT_LISTENER) {
		return fmt.Sprintf("%v", c.Events.Listener), nil
	}

	return "", fmt.Errorf("unknown OID %v", oid)
}

func (c Config) GetOctets(oid scmp.OID) ([]byte, error) {
	if scmp.Is(oid, scmp.OID_CONTROLLER_ADDRESS) {
		addr := c.Network.IPv4.Address.As4()

		return addr[:], nil
	}

	if scmp.Is(oid, scmp.OID_CONTROLLER_NETMASK) {
		return c.Network.IPv4.Netmask, nil
	}

	if scmp.Is(oid, scmp.OID_CONTROLLER_GATEWAY) {
		addr := c.Network.IPv4.Gateway.As4()

		return addr[:], nil
	}

	if scmp.Is(oid, scmp.OID_CONTROLLER_MAC) {
		bytes := c.Network.IPv4.MAC

		return bytes[:], nil
	}

	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (c *Config) SetUint8(oid scmp.OID, val uint8) (uint8, error) {
	if scmp.Is(oid, scmp.OID_CONTROLLER_UPLOAD_INTERVAL) {
		c.Events.Interval = val

		return c.Events.Interval, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c *Config) SetString(oid scmp.OID, val string) (string, error) {
	if scmp.Is(oid, scmp.OID_CONTROLLER_EVENT_LISTENER) {
		if addrPort, err := netip.ParseAddrPort(val); err != nil {
			return "", err
		} else {
			c.Events.Listener = addrPort

			return fmt.Sprintf("%v", c.Events.Listener), nil
		}
	}

	return "", fmt.Errorf("unknown OID %v", oid)
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
