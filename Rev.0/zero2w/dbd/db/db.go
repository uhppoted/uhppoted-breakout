package db

import (
	"fmt"
	"strings"

	"dbd/db/sqlite3"
	"dbd/entities"
)

type DB interface {
	GetDoor(controller uint32, door uint8) (*entities.Door, error)
	SetDoor(controller uint32, door uint8, mode uint8, delay uint8) (*entities.Door, error)
	GetInterlock(controller uint32) (*entities.Interlock, error)
	SetInterlock(controller uint32, interlock uint8) (*entities.Interlock, error)
	GetAntiPassback(controller uint32) (*entities.AntiPassback, error)

	GetCards(controller uint32) (uint32, error)
	GetCard(controller uint32, card uint32) (*entities.Card, error)
	GetCardByIndex(controller uint32, index uint32) (*entities.Card, error)
	PutCard(controller uint32, card entities.Card) (uint32, error)
	DeleteCard(controller uint32, card uint32) (bool, error)
	DeleteAllCards(controller uint32) (bool, error)

	GetEvents(controller uint32) (uint32, uint32, error)
	GetEvent(copntroller uint32, index uint32) (entities.Event, error)
	PutEvent(controller uint32, event entities.Event) (uint32, error)
	GetEventIndex(controller uint32) (uint32, error)
	SetEventIndex(controller uint32, index uint32) error
	RecordSpecialEvents(controller uint32, enabled bool) error
}

var db DB

func Init(dsn string) error {
	switch {
	case strings.HasPrefix(dsn, "sqlite3://"):
		db = sqlite3.Mattn(dsn[10:])
		return nil

	case strings.HasPrefix(dsn, "sqlite://"):
		db = sqlite3.ModernC(dsn[9:])
		return nil

	default:
		return fmt.Errorf("unsupported DSN (%v)", dsn)
	}
}

func GetDoor(controller uint32, door uint8) (*entities.Door, error) {
	return db.GetDoor(controller, door)
}

func SetDoor(controller uint32, door uint8, mode uint8, delay uint8) (*entities.Door, error) {
	return db.SetDoor(controller, door, mode, delay)
}

func GetInterlock(controller uint32) (*entities.Interlock, error) {
	return db.GetInterlock(controller)
}

func GetAntiPassback(controller uint32) (*entities.AntiPassback, error) {
	return db.GetAntiPassback(controller)
}

func SetInterlock(controller uint32, interlock uint8) (*entities.Interlock, error) {
	return db.SetInterlock(controller, interlock)
}

func GetCards(controller uint32) (uint32, error) {
	return db.GetCards(controller)
}

func GetCard(controller uint32, card uint32) (*entities.Card, error) {
	return db.GetCard(controller, card)
}

func GetCardByIndex(controller uint32, index uint32) (*entities.Card, error) {
	return db.GetCardByIndex(controller, index)
}

func PutCard(controller uint32, card entities.Card) (uint32, error) {
	return db.PutCard(controller, card)
}

func DeleteCard(controller uint32, card uint32) (bool, error) {
	return db.DeleteCard(controller, card)
}

func DeleteAllCards(controller uint32) (bool, error) {
	return db.DeleteAllCards(controller)
}

func GetEvents(controller uint32) (uint32, uint32, error) {
	return db.GetEvents(controller)
}

func GetEvent(controller uint32, index uint32) (entities.Event, error) {
	return db.GetEvent(controller, index)
}

func PutEvent(controller uint32, event entities.Event) (uint32, error) {
	return db.PutEvent(controller, event)
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
