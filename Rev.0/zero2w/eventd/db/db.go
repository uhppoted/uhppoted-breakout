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
