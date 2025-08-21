package events

import (
	"fmt"
	"regexp"

	"ut0311/log"
)

const LOGTAG = "events"

type Events struct {
	dial struct {
		network string
		address string
	}

	recordAll bool
	index     uint32
}

func NewEvents(dial string) (*Events, error) {
	infof("init dial:%v", dial)

	e := Events{
		recordAll: false,
	}

	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(dial); len(matches) < 3 {
		return nil, fmt.Errorf("invalid RPC 'dial' address (%v)", dial)
	} else {
		e.dial.network = matches[1]
		e.dial.address = matches[2]
	}

	return &e, nil
}

func debugf(format string, args ...any) {
	log.Debugf(LOGTAG, format, args...)
}

func infof(format string, args ...any) {
	log.Infof(LOGTAG, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(LOGTAG, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(LOGTAG, format, args...)
}
