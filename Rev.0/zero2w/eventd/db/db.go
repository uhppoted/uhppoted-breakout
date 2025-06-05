package db

import (
	"fmt"
	"strings"

	"eventd/db/sqlite3"
	"eventd/entities"
)

type DB interface {
	GetEvents() (uint32, uint32, error)
	GetEvent(index uint32) (entities.Event, error)
	PutEvent(event entities.Event) (uint32, error)
	GetEventIndex(uint32) (uint32, error)
	SetEventIndex(uint32, uint32) error
	RecordSpecialEvents(uint32, bool) error
}

var db DB

func Init(dsn string) error {
	switch {
	case strings.HasPrefix(dsn, "sqlite3://"):
		db = sqlite3.NewDB(dsn[10:])
		return nil

	default:
		return fmt.Errorf("unsupported DSN (%v)", dsn)
	}
}

func GetEvents() (uint32, uint32, error) {
	return db.GetEvents()
}

func GetEvent(index uint32) (entities.Event, error) {
	return db.GetEvent(index)
}

func PutEvent(event entities.Event) (uint32, error) {
	return db.PutEvent(event)
}

func GetEventIndex(controller uint32) (uint32, error) {
	return db.GetEventIndex(controller)
}

func SetEventIndex(controller uint32, index uint32) error {
	return db.SetEventIndex(controller, index)
}

func RecordSpecialEvents(controller uint32, enabled bool) error {
	return db.RecordSpecialEvents(controller, enabled)
}
