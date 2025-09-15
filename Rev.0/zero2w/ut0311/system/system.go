package system

import (
	"fmt"
	"regexp"

	"ut0311/log"
)

const LOGTAG = "system"

type System struct {
	dial struct {
		network string
		address string
	}
}

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
