package cards

import (
	"dbd/db"
	"dbd/entities"
	"dbd/log"
)

const LOGTAG = "cards"

type CardD struct {
}

func (d *CardD) GetCard(args struct {
	Controller uint32
	Card       uint32
}, record *entities.Card) error {
	debugf("get-card %v %v", args.Controller, args.Card)

	if v, err := db.GetCard(args.Controller, args.Card); err != nil {
		return err
	} else {
		*record = v
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
