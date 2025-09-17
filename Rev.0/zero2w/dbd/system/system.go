package system

import (
	"dbd/db"
	"dbd/entities"
	"dbd/log"
)

const LOGTAG = "system"

type System struct {
}

func (s *System) GetDoor(args entities.Door, record *entities.Door) error {
	debugf("get-door %v %v", args.Controller, args.Door)

	if v, err := db.GetDoor(args.Controller, args.Door); err != nil {
		return err
	} else if v != nil {
		*record = *v
	}

	return nil
}

func (s *System) SetDoor(args entities.Door, record *entities.Door) error {
	debugf("set-door %v %v %v %v", args.Controller, args.Door, args.Mode, args.Delay)

	if v, err := db.SetDoor(args.Controller, args.Door, args.Mode, args.Delay); err != nil {
		return err
	} else if v != nil {
		*record = *v
	}

	return nil
}

func (s *System) GetInterlock(args entities.Interlock, record *entities.Interlock) error {
	debugf("get-interlock %v", args.Controller)

	if v, err := db.GetInterlock(args.Controller); err != nil {
		return err
	} else if v != nil {
		*record = *v
	}

	return nil
}

func (s *System) SetInterlock(args entities.Interlock, record *entities.Interlock) error {
	debugf("set-interlock %v %v", args.Controller, args.Interlock)

	if v, err := db.SetInterlock(args.Controller, args.Interlock); err != nil {
		return err
	} else if v != nil {
		*record = *v
	}

	return nil
}

func (s *System) GetAntiPassback(args entities.AntiPassback, record *entities.AntiPassback) error {
	debugf("get-antipassback %v", args.Controller)

	if v, err := db.GetAntiPassback(args.Controller); err != nil {
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
