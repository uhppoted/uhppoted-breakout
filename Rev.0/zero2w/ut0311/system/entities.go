package system

import (
	"time"
)

type Swipe struct {
	Controller uint32
	Card       uint32
	Door       uint8
	Timestamp  time.Time
}
