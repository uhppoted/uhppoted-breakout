package entities

import (
	"fmt"
	"time"
)

type Event struct {
	Index     uint32      `json:"index"`
	Type      EventType   `json:"type"`
	Granted   bool        `json:"granted"`
	Door      uint8       `json:"door"`
	Direction uint8       `json:"direction"`
	Card      uint32      `json:"card"`
	Timestamp time.Time   `json:"timestamp,omitempty"`
	Reason    EventReason `json:"reason"`
}

type EventType uint8
type EventReason uint8

const (
	EventUnknown EventType = 0
	EventCard    EventType = 1
	EventDoor    EventType = 2
	EventAlarm   EventType = 3
	EventSystem  EventType = 4
)

const (
	ReasonUnknown                        EventReason = 0
	ReasonCardOk                                     = 1
	ReasonCardDeniedPCControl                        = 5
	ReasonCardDeniedNoAccess                         = 6
	ReasonCardDeniedPassword                         = 7
	ReasonCardDeniedAntiPassback                     = 8
	ReasonCardDeniedMoreCards                        = 9
	ReasonCardDeniedFirstCardOpen                    = 10
	ReasonCardDeniedDoorNormallyClosed               = 11
	ReasonCardDeniedDoorInterLock                    = 12
	ReasonCardDeniedLimitedTimes                     = 13
	ReasonCardDeniedInvalidTimezone                  = 15
	ReasonCardDenied                                 = 18
	ReasonDoorPushButton                             = 20
	ReasonDoorOpen                                   = 23
	ReasonDoorClosed                                 = 24
	ReasonDoorSupervisorPasswordOpen                 = 25
	ReasonControllerPowerOn                          = 28
	ReasonControllerReset                            = 29
	ReasonPushbuttonDeniedDisabledByTask             = 30
	ReasonPushbuttonDeniedForcedLock                 = 31
	ReasonPushbuttonDeniedNotOnLine                  = 32
	ReasonPushButtonDeniedDoorInterLock              = 33
	ReasonAlarmThreat                                = 34
	ReasonAlarmOpenTooLong                           = 37
	ReasonAlarmForcedOpen                            = 38
	ReasonAlarmFire                                  = 39
	ReasonAlarmForcedClose                           = 40
	ReasonAlarmTamperDetect                          = 41
	ReasonAlarm24x7Zone                              = 42
	ReasonAlarmEmergencyCall                         = 43
	ReasonRemoteOpenDoor                             = 44
	ReasonRemoteOpenDoorUSBReader                    = 45

	// ... custom events
	ReasonDoorUnlocked = 201
	ReasonDoorLocked   = 202
)

func (r EventReason) String() string {
	switch r {
	case ReasonUnknown:
		return "unknown"

	case ReasonCardOk:
		return "card ok"

	case ReasonCardDeniedPCControl:
		return "card denied (PC control)"

	case ReasonCardDeniedNoAccess:
		return "card denied (no access)"

	case ReasonCardDeniedPassword:
		return "card denied (password)"

	case ReasonCardDeniedAntiPassback:
		return "card denied (anti-passback)"

	case ReasonCardDeniedMoreCards:
		return "card denied (more cards)"

	case ReasonCardDeniedFirstCardOpen:
		return "card denied (first card)"

	case ReasonCardDeniedDoorNormallyClosed:
		return "card denied (door normally closed)"

	case ReasonCardDeniedDoorInterLock:
		return "card denied (door interlock)"

	case ReasonCardDeniedLimitedTimes:
		return "card denied (limited times)"

	case ReasonCardDeniedInvalidTimezone:
		return "card denied (invalid timezone)"

	case ReasonCardDenied:
		return "card denied"

	case ReasonDoorPushButton:
		return "door pushbutton"

	case ReasonDoorOpen:
		return "door open"

	case ReasonDoorClosed:
		return "door closed"

	case ReasonDoorSupervisorPasswordOpen:
		return "door open (supervisor)"

	case ReasonControllerPowerOn:
		return "controller power on"

	case ReasonControllerReset:
		return "controller reset"

	case ReasonPushbuttonDeniedDisabledByTask:
		return "pushbutton denied (task disable)"

	case ReasonPushbuttonDeniedForcedLock:
		return "pushbutton denied (forced lock)"

	case ReasonPushbuttonDeniedNotOnLine:
		return "pushbutton denied (offline)"

	case ReasonPushButtonDeniedDoorInterLock:
		return "pushbutton denied (interlock)"

	case ReasonAlarmThreat:
		return "alarm (threat)"

	case ReasonAlarmOpenTooLong:
		return "alarm (open too long)"

	case ReasonAlarmForcedOpen:
		return "alaram (forced open)"

	case ReasonAlarmFire:
		return "alarm (fire)"

	case ReasonAlarmForcedClose:
		return "alarm (forced close)"

	case ReasonAlarmTamperDetect:
		return "alarm (tamper)"

	case ReasonAlarm24x7Zone:
		return "alarm (24x7 zone)"

	case ReasonAlarmEmergencyCall:
		return "alarm (emergency call)"

	case ReasonRemoteOpenDoor:
		return "remote open door"

	case ReasonRemoteOpenDoorUSBReader:
		return "remote open door (USB)"

	case ReasonDoorUnlocked:
		return "door unlocked"

	case ReasonDoorLocked:
		return "door locked"

	default:
		return fmt.Sprintf("other (%v)", r)
	}
}
