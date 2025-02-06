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

type GetI interface {
	GetIndexedUint8(oid OID, index uint32) (uint8, error)
	GetIndexedUint16(oid OID, index uint32) (uint16, error)
	GetIndexedUint32(oid OID, index uint32) (uint32, error)
	GetIndexedBool(oid OID, index uint32) (bool, error)
	GetIndexedString(oid OID, index uint32) (string, error)
	GetIndexedOctets(oid OID, index uint32) ([]byte, error)
	GetIndexedRecord(oid OID, index uint32) (any, error)
}

type SetV interface {
	SetUint8(oid OID, v uint8) (uint8, error)
	SetUint32(oid OID, v uint32) (uint32, error)
	SetUint32A(oid OID, v []uint32) ([]uint32, error)
	SetBool(oid OID, v bool) (bool, error)
	SetString(oid OID, v string) (string, error)
	SetIndexedRecord(oid OID, index uint32, record any) (any, error)
	DeleteIndexedRecord(oid OID, index uint32) (bool, error)
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

func GetIndexed[T any](scmp GetI, oid OID, index uint32) (T, error) {
	var zero T

	switch any(zero).(type) {
	case uint8:
		if v, err := scmp.GetIndexedUint8(oid, index); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case uint32:
		if v, err := scmp.GetIndexedUint32(oid, index); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case bool:
		if v, err := scmp.GetIndexedBool(oid, index); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case types.DateTime:
		if v, err := scmp.GetIndexedString(oid, index); err != nil {
			return zero, err
		} else if datetime, err := types.ParseDateTime(v); err != nil {
			return zero, err
		} else {
			return any(datetime).(T), nil
		}

	case types.Date:
		if v, err := scmp.GetIndexedString(oid, index); err != nil {
			return zero, err
		} else if date, err := types.ParseDate(v); err != nil {
			return zero, err
		} else {
			return any(date).(T), nil
		}

	case Card:
		if v, err := scmp.GetIndexedRecord(oid, index); err != nil {
			return zero, err
		} else if card, ok := v.(Card); !ok {
			return zero, fmt.Errorf("invalid card record (%T)", v)
		} else {
			return any(card).(T), nil
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

	case uint32:
		if u32, err := scmp.SetUint32(oid, v); err != nil {
			return zero, err
		} else {
			return any(u32).(T), nil
		}

	case []uint32:
		if u32a, err := scmp.SetUint32A(oid, v); err != nil {
			return zero, err
		} else {
			return any(u32a).(T), nil
		}

	case bool:
		if b, err := scmp.SetBool(oid, v); err != nil {
			return zero, err
		} else {
			return any(b).(T), nil
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

func SetIndexed[T any](scmp SetV, oid OID, index uint32, val T) (T, error) {
	var zero T

	switch v := any(val).(type) {
	case Card:
		if record, err := scmp.SetIndexedRecord(oid, index, v); err != nil {
			return zero, err
		} else if card, ok := record.(Card); !ok {
			return zero, fmt.Errorf("invalid card record (%T)", record)
		} else {
			return any(card).(T), nil
		}
	}

	return zero, fmt.Errorf("unknown type %T", val)
}

func DeleteIndexed[T any](scmp SetV, oid OID, index uint32) (bool, error) {
	return scmp.DeleteIndexedRecord(oid, index)
}
