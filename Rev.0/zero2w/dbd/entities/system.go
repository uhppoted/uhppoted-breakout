package entities

import ()

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
