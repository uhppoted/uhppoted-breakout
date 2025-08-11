package entities

import (
	"time"
)

type Card struct {
	Card        uint32          `json:"card"`
	StartDate   time.Time       `json:"start-date"`
	EndDate     time.Time       `json:"end-date"`
	Permissions map[uint8]uint8 `json:"permissions"`
	PIN         uint32          `json:"PIN"`
}
