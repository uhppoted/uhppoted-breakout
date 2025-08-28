package system

import (
	"dbd/db"
	"dbd/entities"
	"dbd/log"
)

const LOGTAG = "system"

type System struct {
}

func (s *System) GetDoor(args struct {
	Controller uint32
	Door       uint8
}, record *entities.Door) error {
	debugf("get-door %v %v", args.Controller, args.Door)

	if v, err := db.GetDoor(args.Controller, args.Door); err != nil {
		return err
	} else if v != nil {
		*record = *v
	}

	return nil
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
