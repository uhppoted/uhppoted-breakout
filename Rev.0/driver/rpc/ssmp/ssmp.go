package ssmp

import (
	"fmt"
	"time"

	"ssmp/log"
)

type SSMP struct {
}

func (s SSMP) Get(oid string) (any, error) {
	debugf("get %v", oid)

	// ... controller ID
	if oid == ".1.3.6.1.4.1.65536.2.1" {
		return uint32(405419896), nil
	}

	// ... controller version
	if oid == ".1.3.6.1.4.1.65536.2.2" {
		return uint16(0x1234), nil
	}

	// ... controller release date
	if oid == ".1.3.6.1.4.1.65536.2.3" {
		return "2024-01-15", nil
	}

	// ... controller date/time
	if oid == ".1.3.6.1.4.1.65536.2.8" {
		return time.Now().Format("2006-01-02 15:04:05"), nil
	}

	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (s SSMP) Set(oid string, value any) (any, error) {
	debugf("set %v %v", oid, value)

	// ... controller date/time
	if oid == ".1.3.6.1.4.1.65536.2.8" {
		return time.Now().Format("2006-01-02 15:04:05"), nil
	}

	return nil, fmt.Errorf("unknown OID %v", oid)
}

func debugf(format string, args ...any) {
	log.Debugf("SSMP", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("SSMP", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("SSMP", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("SSMP", format, args...)
}
