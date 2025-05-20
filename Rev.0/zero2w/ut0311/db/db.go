package db

import (
	"fmt"
	"strings"

	"emulator/db/sqlite3"
	"emulator/entities"
)

type DB interface {
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

func Get(index uint32) (entities.Event, error) {
	return db.GetEvent(index)
}

func Put(event entities.Event) (uint32, error) {
	return db.PutEvent(event)
}
