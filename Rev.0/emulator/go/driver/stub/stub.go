package stub

import (
	"fmt"
	"time"

	"emulator/MIB"
	"emulator/log"
)

type Stub struct {
}

type KV struct {
	OID   string
	Value any
}

func (s Stub) get(oid MIB.OID) (any, error) {
	debugf("get %v", oid)

	key := fmt.Sprintf("%v", oid)

	// ... controller ID
	if key == ".1.3.6.1.4.1.65536.2.1" {
		return uint32(405419896), nil
	}

	// ... controller version
	if key == ".1.3.6.1.4.1.65536.2.2" {
		return uint16(0x1234), nil
	}

	// ... controller release date
	if key == ".1.3.6.1.4.1.65536.2.3" {
		return "2024-01-15", nil
	}

	// ... controller date/time
	if key == ".1.3.6.1.4.1.65536.2.8" {
		return time.Now().Format("2006-01-02 15:04:05"), nil
	}

	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (s Stub) GetUint8(oid MIB.OID) (uint8, error) {
	if v, err := s.get(oid); err != nil {
		return 0, err
	} else if u8, ok := v.(uint8); !ok {
		return 0, fmt.Errorf("invalid uint8 value (%T)", v)
	} else {
		return u8, nil
	}
}

func (s Stub) GetUint16(oid MIB.OID) (uint16, error) {
	if v, err := s.get(oid); err != nil {
		return 0, err
	} else if u16, ok := v.(uint16); !ok {
		return 0, fmt.Errorf("invalid uint16 value (%T)", v)
	} else {
		return u16, nil
	}
}

func (s Stub) GetUint32(oid MIB.OID) (uint32, error) {
	if v, err := s.get(oid); err != nil {
		return 0, err
	} else if u32, ok := v.(uint32); !ok {
		return 0, fmt.Errorf("invalid uint32 value (%T)", v)
	} else {
		return u32, nil
	}
}

func (s Stub) GetBool(oid MIB.OID) (bool, error) {
	if v, err := s.get(oid); err != nil {
		return false, err
	} else if b, ok := v.(bool); !ok {
		return false, fmt.Errorf("invalid value - expected 'bool', got %T", v)
	} else {
		return b, nil
	}
}

func (s Stub) GetString(oid MIB.OID) (string, error) {
	if v, err := s.get(oid); err != nil {
		return "", err
	} else if s, ok := v.(string); !ok {
		return "", fmt.Errorf("invalid string value (%T)", v)
	} else {
		return s, nil
	}
}

func (s Stub) Set(oid MIB.OID, value any) (any, error) {
	debugf("set %v %v", oid, value)

	key := fmt.Sprintf("%v", oid)

	// ... controller date/time
	if key == ".1.3.6.1.4.1.65536.2.8" {
		return time.Now().Format("2006-01-02 15:04:05"), nil
	}

	return nil, fmt.Errorf("unknown OID %v", oid)
}

func debugf(format string, args ...any) {
	log.Debugf("STUB", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("STUB", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("STUB", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("STUB", format, args...)
}
