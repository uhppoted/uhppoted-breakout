package entities

import (
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
)
