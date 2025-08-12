package actions

import (
	"ut0311/log"
)

func debugf(tag string, format string, args ...any) {
	log.Debugf(tag, format, args...)
}

func infof(tag string, format string, args ...any) {
	log.Infof(tag, format, args...)
}

func warnf(tag string, format string, args ...any) {
	log.Warnf(tag, format, args...)
}

func errorf(tag string, format string, args ...any) {
	log.Errorf(tag, format, args...)
}
