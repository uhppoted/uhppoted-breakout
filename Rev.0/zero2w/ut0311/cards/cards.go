package cards

import (
	"fmt"
	"regexp"

	"ut0311/log"
)

const LOGTAG = "cards"

type Cards struct {
	dial struct {
		network string
		address string
	}
}

func NewCards(dial string) (*Cards, error) {
	infof("init dial:%v", dial)

	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(dial); len(matches) < 3 {
		return nil, fmt.Errorf("invalid RPC 'dial' address (%v)", dial)
	} else {
		return &Cards{
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
