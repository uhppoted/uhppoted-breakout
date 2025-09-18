package entities

import (
	"time"
)

type Door struct {
	Controller uint32 `json:"controller"`
	Door       uint8  `json:"door"`
	Mode       uint8  `json:"mode"`
	Delay      uint8  `json:"delay"`
}

type Interlock struct {
	Controller uint32 `json:"controller"`
	Interlock  uint8  `json:"interlock"`
}

type AntiPassback struct {
	Controller   uint32 `json:"controller"`
	AntiPassback uint8  `json:"antipassback"`
}

type Swipe struct {
	Controller uint32
	Card       uint32
	Door       uint8
	Timestamp  time.Time
}
