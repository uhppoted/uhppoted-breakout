package entities

import ()

var events = map[struct {
	tag   string
	value any
}]EventType{
	{"controller.system.errors.restart", true}:  EventSystem,
	{"controller.system.errors.watchdog", true}: EventSystem,

	{"controller.door.1.open", false}: EventDoor,
	{"controller.door.2.open", false}: EventDoor,
	{"controller.door.3.open", false}: EventDoor,
	{"controller.door.4.open", false}: EventDoor,

	{"controller.door.1.open", true}: EventDoor,
	{"controller.door.2.open", true}: EventDoor,
	{"controller.door.3.open", true}: EventDoor,
	{"controller.door.4.open", true}: EventDoor,

	{"controller.door.1.button", false}: EventDoor,
	{"controller.door.2.button", false}: EventDoor,
	{"controller.door.3.button", false}: EventDoor,
	{"controller.door.4.button", false}: EventDoor,

	{"controller.door.1.button", true}: EventDoor,
	{"controller.door.2.button", true}: EventDoor,
	{"controller.door.3.button", true}: EventDoor,
	{"controller.door.4.button", true}: EventDoor,

	{"controller.door.1.unlocked", false}: EventDoor,
	{"controller.door.2.unlocked", false}: EventDoor,
	{"controller.door.3.unlocked", false}: EventDoor,
	{"controller.door.4.unlocked", false}: EventDoor,

	{"controller.door.1.unlocked", true}: EventDoor,
	{"controller.door.2.unlocked", true}: EventDoor,
	{"controller.door.3.unlocked", true}: EventDoor,
	{"controller.door.4.unlocked", true}: EventDoor,

	{"controller.door.1.swipe", nil}: EventCard,
	{"controller.door.2.swipe", nil}: EventCard,
	{"controller.door.3.swipe", nil}: EventCard,
	{"controller.door.4.swipe", nil}: EventCard,
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

	"controller.door.1.unlocked": 1,
	"controller.door.2.unlocked": 2,
	"controller.door.3.unlocked": 3,
	"controller.door.4.unlocked": 4,

	"controller.door.1.swipe": 1,
	"controller.door.2.swipe": 2,
	"controller.door.3.swipe": 3,
	"controller.door.4.swipe": 4,
}

var reasons = map[struct {
	tag   string
	value any
}]EventReason{
	{"controller.system.errors.restart", true}:  ReasonControllerPowerOn,
	{"controller.system.errors.watchdog", true}: ReasonControllerReset,

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

	{"controller.door.1.unlocked", true}: ReasonDoorUnlocked,
	{"controller.door.2.unlocked", true}: ReasonDoorUnlocked,
	{"controller.door.3.unlocked", true}: ReasonDoorUnlocked,
	{"controller.door.4.unlocked", true}: ReasonDoorUnlocked,

	{"controller.door.1.unlocked", false}: ReasonDoorLocked,
	{"controller.door.2.unlocked", false}: ReasonDoorLocked,
	{"controller.door.3.unlocked", false}: ReasonDoorLocked,
	{"controller.door.4.unlocked", false}: ReasonDoorLocked,

	{"controller.door.1.swipe", nil}: ReasonCardOk,
	{"controller.door.2.swipe", nil}: ReasonCardOk,
	{"controller.door.3.swipe", nil}: ReasonCardOk,
	{"controller.door.4.swipe", nil}: ReasonCardOk,
}

func LookupEvent(tag string, value any) EventType {
	key := struct {
		tag   string
		value any
	}{
		tag:   tag,
		value: value,
	}

	if v, ok := events[key]; ok {
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
