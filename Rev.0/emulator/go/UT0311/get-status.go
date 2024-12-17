package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
	"emulator/driver"
)

func (ut0311 *UT0311) getStatus(rq *messages.GetStatusRequest) (any, error) {
	if id, err := driver.Get[uint32](ut0311.driver, MIB.OID_CONTROLLER_ID); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetStatusResponse{
			SerialNumber: types.SerialNumber(id),
			RelayState:   0x00,
		}

		if datetime, err := driver.Get[types.DateTime](ut0311.driver, MIB.OID_CONTROLLER_DATETIME); err != nil {
			return nil, err
		} else {
			response.SystemDate = types.SystemDate(datetime)
			response.SystemTime = types.SystemTime(datetime)
		}

		if v, err := driver.Get[uint8](ut0311.driver, MIB.OID_CONTROLLER_ERROR); err != nil {
			return nil, err
		} else {
			response.SystemError = v
		}

		if v, err := driver.Get[uint8](ut0311.driver, MIB.OID_CONTROLLER_SPECIAL_INFO); err != nil {
			return nil, err
		} else {
			response.SpecialInfo = v
		}

		// ... door 1
		if unlocked, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_1_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked {
			response.RelayState |= 0x01
		}

		if open, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_1_OPEN); err != nil {
			return nil, err
		} else {
			response.Door1State = open
		}

		if pressed, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_1_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door1Button = pressed
		}

		// ... door 2
		if unlocked, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_2_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked {
			response.RelayState |= 0x02
		}

		if open, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_2_OPEN); err != nil {
			return nil, err
		} else {
			response.Door2State = open
		}

		if pressed, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_2_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door2Button = pressed
		}

		// ... door 3
		if unlocked, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_3_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked {
			response.RelayState |= 0x04
		}

		if open, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_3_OPEN); err != nil {
			return nil, err
		} else {
			response.Door3State = open
		}

		if pressed, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_3_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door3Button = pressed
		}

		// ... door 4
		if unlocked, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_4_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked {
			response.RelayState |= 0x08
		}

		if open, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_4_OPEN); err != nil {
			return nil, err
		} else {
			response.Door4State = open
		}

		if pressed, err := driver.Get[bool](ut0311.driver, MIB.OID_DOORS_4_BUTTON); err != nil {
			return nil, err
		} else {
			response.Door4Button = pressed
		}

		return response, nil
	}
}
