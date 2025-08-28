package system

import (
	"fmt"
	"regexp"

	"ut0311/log"
	"ut0311/scmp"
)

type System struct {
	dial struct {
		network string
		address string
	}
}

const LOGTAG = "system"

func NewSystem(dial string) (*System, error) {
	infof("init dial:%v", dial)

	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(dial); len(matches) < 3 {
		return nil, fmt.Errorf("invalid RPC 'dial' address (%v)", dial)
	} else {
		return &System{
			dial: struct {
				network string
				address string
			}{
				network: matches[1],
				address: matches[2],
			},
		}, nil
	}
}

func (sys System) GetUint8(oid scmp.OID) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetUint16(oid scmp.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetUint32(oid scmp.OID) (uint32, error) {
	if scmp.Is(oid, scmp.OID_CONTROLLER_SEQUENCE_NUMBER) {
		return 0, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetBool(oid scmp.OID) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetString(oid scmp.OID) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func (sys System) GetOctets(oid scmp.OID) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (sys *System) SetUint8(oid scmp.OID, val uint8) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (sys *System) SetString(oid scmp.OID, value string) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func debugf(format string, args ...any) {
	log.Debugf("SYS", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("SYS", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("SYS", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("SYS", format, args...)
}
