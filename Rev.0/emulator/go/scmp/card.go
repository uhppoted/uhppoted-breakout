package scmp

import (
	"github.com/uhppoted/uhppote-core/types"
)

type Card struct {
	Card        uint32
	StartDate   types.Date
	EndDate     types.Date
	Permissions map[uint8]uint8
	PIN         uint32
}
