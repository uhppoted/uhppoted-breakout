package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	lib "github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

const errMemory uint16 = 0x0001
const errQueue uint16 = 0x0002
const errI2C uint16 = 0x0004
const errRX8900 uint16 = 0x0008
const errU2 uint16 = 0x0010
const errU3 uint16 = 0x0020
const errU4 uint16 = 0x0040
const errWatchdog uint16 = 0x0080
const errDebug uint16 = 0x4000
const errUnknown uint16 = 0x8000

func (ut0311 *UT0311) getStatus(rq *messages.GetStatusRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.breakout, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetStatusResponse{
			SerialNumber: lib.SerialNumber(id),
			RelayState:   0x00,
			InputState:   0x00,
		}

		if datetime, err := scmp.Get[lib.DateTime](ut0311.breakout, scmp.OID_CONTROLLER_DATETIME); err != nil {
			return nil, err
		} else {
			response.SystemDate = lib.SystemDate(datetime)
			response.SystemTime = lib.SystemTime(datetime)
		}

		if v, err := scmp.Get[uint16](ut0311.breakout, scmp.OID_CONTROLLER_SYSERR); err != nil {
			return nil, err
		} else {
			syserr := uint8(0x00)

			if (v & (errMemory | errQueue | errI2C | errRX8900 | errU2 | errU3 | errU4)) != 0x0000 {
				syserr |= 0x01
			}

			if (v & errWatchdog) != 0x0000 {
				syserr |= 0x02
			}

			if (v & errUnknown) != 0x0000 {
				syserr |= 0x08
			}

			response.SystemError = syserr
		}

		if v, err := scmp.Get[uint8](ut0311.breakout, scmp.OID_CONTROLLER_SYSINFO); err != nil {
			return nil, err
		} else {
			response.SpecialInfo = v
		}

		if v, err := scmp.Get[uint32](ut0311.system, scmp.OID_CONTROLLER_SEQUENCE_NUMBER); err != nil {
			return nil, err
		} else {
			response.SequenceId = v
		}

		// ... door 1
		if locked, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_1_UNLOCKED); err != nil {
			return nil, err
		} else if !locked {
			response.RelayState |= 0x01
		}

		if open, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_1_OPEN); err != nil {
			return nil, err
		} else {
			response.Door1State = open
		}

		if pressed, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_1_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door1Button = pressed
		}

		// ... door 2
		if locked, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_2_UNLOCKED); err != nil {
			return nil, err
		} else if !locked {
			response.RelayState |= 0x02
		}

		if open, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_2_OPEN); err != nil {
			return nil, err
		} else {
			response.Door2State = open
		}

		if pressed, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_2_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door2Button = pressed
		}

		// ... door 3
		if locked, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_3_UNLOCKED); err != nil {
			return nil, err
		} else if !locked {
			response.RelayState |= 0x04
		}

		if open, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_3_OPEN); err != nil {
			return nil, err
		} else {
			response.Door3State = open
		}

		if pressed, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_3_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door3Button = pressed
		}

		// ... door 4
		if locked, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_4_UNLOCKED); err != nil {
			return nil, err
		} else if !locked {
			response.RelayState |= 0x08
		}

		if open, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_4_OPEN); err != nil {
			return nil, err
		} else {
			response.Door4State = open
		}

		if pressed, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_4_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door4Button = pressed
		}

		// ... inputs
		if tampered, err := scmp.Get[bool](ut0311.breakout, scmp.OID_ALARMS_TAMPER_DETECT); err != nil {
			return nil, err
		} else if tampered {
			response.InputState |= 0x01
		}

		if fire, err := scmp.Get[bool](ut0311.breakout, scmp.OID_ALARMS_FIRE_ALARM); err != nil {
			return nil, err
		} else if fire {
			response.InputState |= 0x02
		}

		// ... event
		if event, err := ut0311.events.Get(0xffffffff); err != nil {
			return nil, err
		} else {
			response.EventIndex = event.Index
			response.EventType = uint8(event.Type)
			response.Granted = event.Granted
			response.Door = event.Door
			response.Direction = event.Direction
			response.CardNumber = event.Card
			response.Timestamp = lib.DateTime(event.Timestamp)
			response.Reason = uint8(event.Reason)
		}

		return response, nil
	}
}
