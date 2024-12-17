package events

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
)

type Events interface {
	GetUint8(oid MIB.OID) (uint8, error)
	GetUint16(oid MIB.OID) (uint16, error)
	GetUint32(oid MIB.OID) (uint32, error)
	GetBool(oid MIB.OID) (bool, error)
	GetString(oid MIB.OID) (string, error)
}

func Get[T any](events Events, oid MIB.OID) (T, error) {
	var zero T

	switch any(zero).(type) {
	case uint8:
		if v, err := events.GetUint8(oid); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case uint16:
		if v, err := events.GetUint16(oid); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case uint32:
		if v, err := events.GetUint32(oid); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case bool:
		if v, err := events.GetBool(oid); err != nil {
			return zero, err
		} else {
			return any(v).(T), nil
		}

	case types.Date:
		if v, err := events.GetString(oid); err != nil {
			return zero, err
		} else if date, err := types.ParseDate(v); err != nil {
			return zero, err
		} else {
			return any(date).(T), nil
		}

	case types.DateTime:
		if v, err := events.GetString(oid); err != nil {
			return zero, err
		} else if datetime, err := types.ParseDateTime(v); err != nil {
			return zero, err
		} else {
			return any(datetime).(T), nil
		}
	}

	return zero, fmt.Errorf("unknown type %T", zero)
}
