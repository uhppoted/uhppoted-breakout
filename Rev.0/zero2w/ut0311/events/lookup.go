package events

import (
	"emulator/scmp"
)

type EventType uint8
type Reason uint8

const (
	EventUnknown EventType = 0
	EventCard    EventType = 1
	EventDoor    EventType = 2
	EventAlarm   EventType = 3
)

const (
	ReasonUnknown                      Reason = 0
	ReasonCardOk                              = 1
	ReasonCardDeniedPCControl                 = 5
	ReasonCardDeniedNoAccess                  = 6
	ReasonCardDeniedPassword                  = 7
	ReasonCardDeniedAntiPassback              = 8
	ReasonCardDeniedMoreCards                 = 9
	ReasonCardDeniedFirstCardOpen             = 10
	ReasonCardDeniedDoorNormallyClosed        = 11
	ReasonCardDeniedDoorInterLock             = 12
	ReasonCardDeniedLimitedTimes              = 13
	ReasonCardDeniedInvalidTimezone           = 15
	ReasonCardDenied                          = 18
	ReasonDoorPushButton                      = 20
	ReasonDoorOpen                            = 23
	ReasonDoorClosed                          = 24
	ReasonDoorSupervisorPasswordOpen25
	ReasonControllerPowerOn              = 28
	ReasonControllerReset                = 29
	ReasonPushbuttonDeniedDisabledByTask = 30
	ReasonPushbuttonDeniedForcedLock     = 31
	ReasonPushbuttonDeniedNotOnLine      = 32
	ReasonPushButtonDeniedDoorInterLock  = 33
	ReasonAlarmThreat                    = 34
	ReasonAlarmOpenTooLong               = 37
	ReasonAlarmForcedOpen                = 38
	ReasonAlarmFire                      = 39
	ReasonAlarmForcedClose               = 40
	ReasonAlarmTamperDetect              = 41
	ReasonAlarm24x7Zone                  = 42
	ReasonAlarmEmergencyCall             = 43
	ReasonRemoteOpenDoor                 = 44
	ReasonRemoteOpenDoorUSBReader        = 45
)

func lookup(OID string) EventType {
	if oid, err := scmp.Parse(OID); err == nil {
		doors := []scmp.OID{
			scmp.OID_DOORS_1_OPEN, scmp.OID_DOORS_1_BUTTON, scmp.OID_DOORS_1_UNLOCKED,
			scmp.OID_DOORS_2_OPEN, scmp.OID_DOORS_2_BUTTON, scmp.OID_DOORS_2_UNLOCKED,
			scmp.OID_DOORS_3_OPEN, scmp.OID_DOORS_3_BUTTON, scmp.OID_DOORS_3_UNLOCKED,
			scmp.OID_DOORS_4_OPEN, scmp.OID_DOORS_4_BUTTON, scmp.OID_DOORS_4_UNLOCKED,
		}

		for _, v := range doors {
			if scmp.Equal(oid, v) {
				return EventDoor
			}
		}
	}

	return EventUnknown
}

func door(OID string) uint8 {
	if oid, err := scmp.Parse(OID); err == nil {
		doors := map[uint8][]scmp.OID{
			1: {scmp.OID_DOORS_1_OPEN, scmp.OID_DOORS_1_BUTTON, scmp.OID_DOORS_1_UNLOCKED},
			2: {scmp.OID_DOORS_2_OPEN, scmp.OID_DOORS_2_BUTTON, scmp.OID_DOORS_2_UNLOCKED},
			3: {scmp.OID_DOORS_3_OPEN, scmp.OID_DOORS_3_BUTTON, scmp.OID_DOORS_3_UNLOCKED},
			4: {scmp.OID_DOORS_4_OPEN, scmp.OID_DOORS_4_BUTTON, scmp.OID_DOORS_4_UNLOCKED},
		}

		for k, v := range doors {
			for _, u := range v {
				if scmp.Equal(oid, u) {
					return k
				}
			}
		}
	}

	return 0
}

func reason(OID string, value any) Reason {
	return ReasonUnknown
}
