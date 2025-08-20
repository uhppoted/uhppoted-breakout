package cards

import (
	"time"

	"dbd/db"
	"dbd/entities"
	"dbd/log"
)

const LOGTAG = "cards"

type CardD struct {
}

func (d *CardD) GetCards(controller uint32, cards *uint32) error {
	debugf("get-cards %v", controller)

	if v, err := db.GetCards(controller); err != nil {
		return err
	} else {
		*cards = v
	}

	return nil
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

func (d *CardD) PutCard(args struct {
	Controller uint32
	Card       uint32
	StartDate  time.Time
	EndDate    time.Time
	Door1      uint8
	Door2      uint8
	Door3      uint8
	Door4      uint8
	PIN        uint32
}, index *uint32) error {
	debugf("put-card %v %v", args.Controller, args.Card)

	card := entities.Card{
		Card:      args.Card,
		StartDate: args.StartDate,
		EndDate:   args.EndDate,
		Permissions: map[uint8]uint8{
			1: args.Door1,
			2: args.Door2,
			3: args.Door3,
			4: args.Door4,
		},
		PIN: args.PIN,
	}

	if ix, err := db.PutCard(args.Controller, card); err != nil {
		return err
	} else {
		*index = ix
	}

	return nil
}

func (d *CardD) DeleteCard(args struct {
	Controller uint32
	Card       uint32
}, ok *bool) error {
	debugf("delete-card %v %v", args.Controller, args.Card)

	if v, err := db.DeleteCard(args.Controller, args.Card); err != nil {
		return err
	} else {
		*ok = v
	}

	return nil
}

func (d *CardD) DeleteAllCards(controller uint32, ok *bool) error {
	debugf("delete-all-cards %v", controller)

	if v, err := db.DeleteAllCards(controller); err != nil {
		return err
	} else {
		*ok = v
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
