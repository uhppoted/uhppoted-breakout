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
	ReasonUnknown Reason = 0
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

// Reason Codes
// 1    01  SwipePass   Swipe
// 2    02  (Reserved)
// 3    03  (Reserved)
// 4    04  (Reserved)
// 5    05  SwipeNOPass Denied Access: PC Control
// 6    06  SwipeNOPass Denied Access: No PRIVILEGE
// 7    07  SwipeNOPass Denied Access: Wrong PASSWORD
// 8    08  SwipeNOPass Denied Access: AntiBack
// 9    09  SwipeNOPass Denied Access: More Cards
// 10   0A  SwipeNOPass Denied Access: First Card Open
// 11   0B  SwipeNOPass Denied Access: Door Set NC
// 12   0C  SwipeNOPass Denied Access: InterLock
// 13   0D  SwipeNOPass Denied Access: Limited Times
// 14   0E  (Reserved)
// 15   0F  SwipeNOPass Denied Access: Invalid Timezone
// 16   10  (Reserved)
// 17   11  (Reserved)
// 18   12  SwipeNOPass Denied Access
// 19   13  (Reserved)
// 20   14  ValidEvent  Push Button
// 21   15  (Reserved)
// 22   16  (Reserved)
// 23   17  ValidEvent  Door Open
// 24   18  ValidEvent  Door Closed
// 25   19  ValidEvent  Super Password Open Door
// 26   1A  (Reserved)
// 27   1B  (Reserved)
// 28   1C  Warn    Controller Power On
// 29   1D  Warn    Controller Reset
// 30   1E  (Reserved | Pushbutton disabled by task)
// 31   1F  Warn    Push Button Invalid: Forced Lock
// 32   20  Warn    Push Button Invalid: Not On Line
// 33   21  Warn    Push Button Invalid: InterLock
// 34   22  Warn    Threat
// 35   23  (Reserved)
// 36   24  (Reserved)
// 37   25  Warn    Open too long
// 38   26  Warn    Forced Open
// 39   27  Warn    Fire
// 40   28  Warn    Forced Close
// 41   29  Warn    Guard Against Theft
// 42   2A  Warn    7*24Hour Zone
// 43   2B  Warn    Emergency Call
// 44   2C  RemoteOpen  Remote Open Door
// 45   2D  RemoteOpen  Remote Open Door By USB Reader
