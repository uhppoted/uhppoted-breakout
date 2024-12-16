package UT0311

import (
	"fmt"

	"github.com/uhppoted/uhppote-core/messages"
	"github.com/uhppoted/uhppote-core/types"

	"emulator/MIB"
)

func (ut0311 *UT0311) getStatus(rq *messages.GetStatusRequest) (any, error) {
	if id, err := ut0311.getID(); err != nil {
		return nil, err
	} else if id == 0 || (rq.SerialNumber != 0 && uint32(rq.SerialNumber) != id) {
		return nil, nil
	} else {
		response := messages.GetStatusResponse{
			SerialNumber: types.SerialNumber(id),
			RelayState:   0x00,
		}

		if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_DATETIME); err != nil {
			return nil, err
		} else if s, ok := v.(string); !ok {
			return nil, fmt.Errorf("invalid controller date/time (%v)", v)
		} else if datetime, err := types.ParseDateTime(s); err != nil {
			return nil, err
		} else {
			response.SystemDate = types.SystemDate(datetime)
			response.SystemTime = types.SystemTime(datetime)
		}

		if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_ERROR); err != nil {
			return nil, err
		} else if e, ok := v.(int); !ok {
			return nil, fmt.Errorf("invalid controller error (%T)", v)
		} else {
			response.SystemError = uint8(e)
		}

		if v, err := ut0311.driver.Get(MIB.OID_CONTROLLER_SPECIAL_INFO); err != nil {
			return nil, err
		} else if e, ok := v.(int); !ok {
			return nil, fmt.Errorf("invalid controller special info (%T)", v)
		} else {
			response.SpecialInfo = uint8(e)
		}

		// ... door 1
		if v, err := ut0311.driver.Get(MIB.OID_DOORS_1_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 1 unlocked (%T)", v)
		} else if unlocked {
			response.RelayState |= 0x01
		}

		if v, err := ut0311.driver.Get(MIB.OID_DOORS_1_OPEN); err != nil {
			return nil, err
		} else if open, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 1 open (%T)", v)
		} else {
			response.Door1State = open
		}

		if v, err := ut0311.driver.Get(MIB.OID_DOORS_1_BUTTON); err != nil {
			return nil, err
		} else if pressed, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 1 button (%T)", v)
		} else {
			response.Door1Button = pressed
		}

		// ... door 2
		if v, err := ut0311.driver.Get(MIB.OID_DOORS_2_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 2 unlocked (%T)", v)
		} else if unlocked {
			response.RelayState |= 0x02
		}

		if v, err := ut0311.driver.Get(MIB.OID_DOORS_2_OPEN); err != nil {
			return nil, err
		} else if open, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 2 open (%T)", v)
		} else {
			response.Door2State = open
		}

		if v, err := ut0311.driver.Get(MIB.OID_DOORS_2_BUTTON); err != nil {
			return nil, err
		} else if pressed, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 2 button (%T)", v)
		} else {
			response.Door2Button = pressed
		}

		// ... door 3
		if v, err := ut0311.driver.Get(MIB.OID_DOORS_3_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 3 unlocked (%T)", v)
		} else if unlocked {
			response.RelayState |= 0x04
		}

		if v, err := ut0311.driver.Get(MIB.OID_DOORS_3_OPEN); err != nil {
			return nil, err
		} else if open, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 3 open (%T)", v)
		} else {
			response.Door3State = open
		}

		if v, err := ut0311.driver.Get(MIB.OID_DOORS_3_BUTTON); err != nil {
			return nil, err
		} else if pressed, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 3 button (%T)", v)
		} else {
			response.Door3Button = pressed
		}

		// ... door 4
		if v, err := ut0311.driver.Get(MIB.OID_DOORS_4_UNLOCKED); err != nil {
			return nil, err
		} else if unlocked, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 4 unlocked (%T)", v)
		} else if unlocked {
			response.RelayState |= 0x08
		}

		if v, err := ut0311.driver.Get(MIB.OID_DOORS_4_OPEN); err != nil {
			return nil, err
		} else if open, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 4 open (%T)", v)
		} else {
			response.Door4State = open
		}

		if v, err := ut0311.driver.Get(MIB.OID_DOORS_4_BUTTON); err != nil {
			return nil, err
		} else if pressed, ok := v.(bool); !ok {
			return nil, fmt.Errorf("invalid controller door 4 button (%T)", v)
		} else {
			response.Door4Button = pressed
		}

		return response, nil
	}
}
