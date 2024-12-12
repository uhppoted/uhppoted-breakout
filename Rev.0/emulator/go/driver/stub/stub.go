package stub

import (
	"fmt"
	"time"

	"emulator/MIB/types"
	"emulator/log"
)

type Stub struct {
}

type KV struct {
	OID   string
	Value any
}

func (s Stub) Get(oid types.OID) (any, error) {
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

func (s Stub) Set(oid types.OID, value any) (any, error) {
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
