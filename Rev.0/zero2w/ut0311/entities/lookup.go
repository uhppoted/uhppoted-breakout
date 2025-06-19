package entities

import ()

var events = map[string]EventType{
	"controller.door.1.open": EventDoor,
	"controller.door.2.open": EventDoor,
	"controller.door.3.open": EventDoor,
	"controller.door.4.open": EventDoor,

	"controller.door.1.button": EventDoor,
	"controller.door.2.button": EventDoor,
	"controller.door.3.button": EventDoor,
	"controller.door.4.button": EventDoor,
}

var doors = map[string]uint8{
	"controller.door.1.open": 1,
	"controller.door.2.open": 2,
	"controller.door.3.open": 3,
	"controller.door.4.open": 4,

	"controller.door.1.button": 1,
	"controller.door.2.button": 2,
	"controller.door.3.button": 3,
	"controller.door.4.button": 4,
}

var reasons = map[struct {
	tag   string
	value any
}]EventReason{
	{"controller.door.1.open", false}: ReasonDoorClosed,
	{"controller.door.2.open", false}: ReasonDoorClosed,
	{"controller.door.3.open", false}: ReasonDoorClosed,
	{"controller.door.4.open", false}: ReasonDoorClosed,

	{"controller.door.1.open", true}: ReasonDoorOpen,
	{"controller.door.2.open", true}: ReasonDoorOpen,
	{"controller.door.3.open", true}: ReasonDoorOpen,
	{"controller.door.4.open", true}: ReasonDoorOpen,

	{"controller.door.1.button", false}: ReasonDoorPushButton,
	{"controller.door.2.button", false}: ReasonDoorPushButton,
	{"controller.door.3.button", false}: ReasonDoorPushButton,
	{"controller.door.4.button", false}: ReasonDoorPushButton,

	{"controller.door.1.button", true}: ReasonDoorPushButton,
	{"controller.door.2.button", true}: ReasonDoorPushButton,
	{"controller.door.3.button", true}: ReasonDoorPushButton,
	{"controller.door.4.button", true}: ReasonDoorPushButton,
}

func LookupEvent(tag string) EventType {
	if v, ok := events[tag]; ok {
		return v
	}

	return EventUnknown
}

func LookupEventDoor(tag string) uint8 {
	if v, ok := doors[tag]; ok {
		return v
	}

	return 0
}

func LookupEventReason(tag string, value any) EventReason {
	key := struct {
		tag   string
		value any
	}{
		tag:   tag,
		value: value,
	}

	if v, ok := reasons[key]; ok {
		return v
	}

	return ReasonUnknown
}
