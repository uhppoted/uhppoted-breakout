package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) getStatus(rq *messages.GetStatusRequest) (any, error) {
	if id, err := scmp.Get[uint32](ut0311.driver, scmp.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetStatusResponse{
			SerialNumber: types.SerialNumber(id),
			RelayState:   0x00,
			InputState:   0x00,
		}

		if datetime, err := scmp.Get[types.DateTime](ut0311.driver, scmp.OID_CONTROLLER_DATETIME); err != nil {
			return nil, err
		} else {
			response.SystemDate = types.SystemDate(datetime)
			response.SystemTime = types.SystemTime(datetime)
		}

		if v, err := scmp.Get[uint8](ut0311.driver, scmp.OID_CONTROLLER_ERROR); err != nil {
			return nil, err
		} else {
			response.SystemError = v
		}

		if v, err := scmp.Get[uint8](ut0311.driver, scmp.OID_CONTROLLER_SPECIAL_INFO); err != nil {
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
		if unlocked, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_1_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked {
			response.RelayState |= 0x01
		}

		if open, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_1_OPEN); err != nil {
			return nil, err
		} else {
			response.Door1State = open
		}

		if pressed, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_1_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door1Button = pressed
		}

		// ... door 2
		if unlocked, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_2_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked {
			response.RelayState |= 0x02
		}

		if open, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_2_OPEN); err != nil {
			return nil, err
		} else {
			response.Door2State = open
		}

		if pressed, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_2_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door2Button = pressed
		}

		// ... door 3
		if unlocked, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_3_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked {
			response.RelayState |= 0x04
		}

		if open, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_3_OPEN); err != nil {
			return nil, err
		} else {
			response.Door3State = open
		}

		if pressed, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_3_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door3Button = pressed
		}

		// ... door 4
		if unlocked, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_4_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked {
			response.RelayState |= 0x08
		}

		if open, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_4_OPEN); err != nil {
			return nil, err
		} else {
			response.Door4State = open
		}

		if pressed, err := scmp.Get[bool](ut0311.driver, scmp.OID_DOORS_4_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door4Button = pressed
		}

		// ... inputs
		if tampered, err := scmp.Get[bool](ut0311.driver, scmp.OID_INPUTS_TAMPER_DETECT); err != nil {
			return nil, err
		} else if tampered {
			response.InputState |= 0x01
		}

		if fire, err := scmp.Get[bool](ut0311.driver, scmp.OID_INPUTS_FIRE_ALARM); err != nil {
			return nil, err
		} else if fire {
			response.InputState |= 0x02
		}

		// ... event
		if index, err := scmp.Get[uint32](ut0311.events, scmp.OID_EVENTS_CURRENT); err != nil {
			return nil, err
		} else if index > 0 {
			response.EventIndex = index

			if event, err := scmp.Get[uint8](ut0311.events, scmp.Indexed(scmp.OID_EVENTS_EVENT_EVENT, index)); err != nil {
				return nil, err
			} else {
				response.EventType = event
			}

			if granted, err := scmp.Get[bool](ut0311.events, scmp.Indexed(scmp.OID_EVENTS_EVENT_GRANTED, index)); err != nil {
				return nil, err
			} else {
				response.Granted = granted
			}

			if door, err := scmp.Get[uint8](ut0311.events, scmp.Indexed(scmp.OID_EVENTS_EVENT_DOOR, index)); err != nil {
				return nil, err
			} else {
				response.Door = door
			}

			if direction, err := scmp.Get[uint8](ut0311.events, scmp.Indexed(scmp.OID_EVENTS_EVENT_DIRECTION, index)); err != nil {
				return nil, err
			} else {
				response.Direction = direction
			}

			if card, err := scmp.Get[uint32](ut0311.events, scmp.Indexed(scmp.OID_EVENTS_EVENT_CARD, index)); err != nil {
				return nil, err
			} else {
				response.CardNumber = card
			}

			if timestamp, err := scmp.Get[types.DateTime](ut0311.events, scmp.Indexed(scmp.OID_EVENTS_EVENT_TIMESTAMP, index)); err != nil {
				return nil, err
			} else {
				response.Timestamp = timestamp
			}

			if reason, err := scmp.Get[uint8](ut0311.events, scmp.Indexed(scmp.OID_EVENTS_EVENT_REASON, index)); err != nil {
				return nil, err
			} else {
				response.Reason = reason
			}
		}

		return response, nil
	}
}
