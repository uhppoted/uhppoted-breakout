package scmp

import (
	"fmt"
	"net"
	"net/netip"

	"github.com/uhppoted/uhppote-core/types"
)

type GetV interface {
	GetUint8(oid OID) (uint8, error)
	GetUint16(oid OID) (uint16, error)
	GetUint32(oid OID) (uint32, error)
	GetBool(oid OID) (bool, error)
	GetString(oid OID) (string, error)
	GetOctets(oid OID) ([]byte, error)
}

type SetV interface {
	SetUint8(oid OID, v uint8) (uint8, error)
	SetString(oid OID, v string) (string, error)
}

func Get[T any](scmp GetV, oid OID) (T, error) {
	var zero T

	switch any(zero).(type) {
	case uint8:
		if v, err := scmp.GetUint8(oid); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case uint16:
		if v, err := scmp.GetUint16(oid); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case uint32:
		if v, err := scmp.GetUint32(oid); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case bool:
		if v, err := scmp.GetBool(oid); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case netip.Addr:
		if v, err := scmp.GetOctets(oid); err != nil {
			return zero, err
		} else if addr, ok := netip.AddrFromSlice(v); !ok {
			return zero, fmt.Errorf("invalid IPv4 address (%v)", v)
		} else {
			return any(addr).(T), nil
		}

	case netip.AddrPort:
		if v, err := scmp.GetString(oid); err != nil {
			return zero, err
		} else if addr, err := netip.ParseAddrPort(v); err != nil {
			return zero, err
		} else {
			return any(addr).(T), nil
		}

	case net.IPMask:
		if v, err := scmp.GetOctets(oid); err != nil {
			return zero, err
		} else if len(v) != 4 {
			return zero, fmt.Errorf("invalid IPv4 netmask (%v)", v)
		} else {
			netmask := net.IPv4Mask(v[0], v[1], v[2], v[3])

			return any(netmask).(T), nil
		}

	case net.HardwareAddr:
		if v, err := scmp.GetOctets(oid); err != nil {
			return zero, err
		} else if len(v) != 6 {
			return zero, fmt.Errorf("invalid MAC address (%v)", v)
		} else {
			MAC := net.HardwareAddr(v)

			return any(MAC).(T), nil
		}

	case types.Date:
		if v, err := scmp.GetString(oid); err != nil {
			return zero, err
		} else if date, err := types.ParseDate(v); err != nil {
			return zero, err
		} else {
			return any(date).(T), nil
		}

	case types.DateTime:
		if v, err := scmp.GetString(oid); err != nil {
			return zero, err
		} else if datetime, err := types.ParseDateTime(v); err != nil {
			return zero, err
		} else {
			return any(datetime).(T), nil
		}
	}

	return zero, fmt.Errorf("unknown type %T", zero)
}

func Set[T any](scmp SetV, oid OID, val T) (T, error) {
	var zero T

	switch v := any(val).(type) {
	case uint8:
		if u8, err := scmp.SetUint8(oid, v); err != nil {
			return zero, err
		} else {
			return any(u8).(T), nil
		}

	case netip.AddrPort:
		if s, err := scmp.SetString(oid, fmt.Sprintf("%v", v)); err != nil {
			return zero, err
		} else if addr, err := netip.ParseAddrPort(s); err != nil {
			return zero, err
		} else {
			return any(addr).(T), nil
		}

	case types.DateTime:
		if s, err := scmp.SetString(oid, fmt.Sprintf("%v", v)); err != nil {
			return zero, err
		} else if datetime, err := types.ParseDateTime(s); err != nil {
			return zero, err
		} else {
			return any(datetime).(T), nil
		}
	}

	return zero, fmt.Errorf("unknown type %T", val)
}
