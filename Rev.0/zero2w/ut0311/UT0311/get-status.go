package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	lib "github.com/uhppoted/uhppote-core/types"

	"ut0311/entities"
	"ut0311/scmp"
)

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
			SystemError:  0x00,
			SpecialInfo:  0x00,
			SequenceId:   0,
		}

		if v, err := ut0311.state.DateTime(); err != nil {
			warnf("%v", err)
		} else {
			response.SystemDate = lib.SystemDate(v)
			response.SystemTime = lib.SystemTime(v)
		}

		// ... system errors
		errs := []uint16{entities.ErrMemory, entities.ErrQueue, entities.ErrI2C, entities.ErrRX8900, entities.ErrU2, entities.ErrU3, entities.ErrU4}
		if v, err := ut0311.state.SystemError(errs...); err != nil {
			warnf("%v", err)
		} else if v {
			response.SystemError |= 0x01
		}

		if v, err := ut0311.state.SystemError(entities.ErrWatchdog); err != nil {
			warnf("%v", err)
		} else if v {
			response.SystemError |= 0x02
		}

		if v, err := ut0311.state.SystemError(entities.ErrUnknown); err != nil {
			warnf("%v", err)
		} else if v {
			response.SystemError |= 0x08
		}

		// ... special info
		if v, err := ut0311.state.SpecialInfo(); err != nil {
			warnf("%v", err)
		} else {
			response.SpecialInfo = v
		}

		// ... sequence no.
		if v, err := ut0311.state.SequenceNo(); err != nil {
			warnf("%v", err)
		} else {
			response.SequenceId = v
		}

		// ... door 1
		if unlocked, err := ut0311.state.DoorUnlocked(1); err != nil {
			warnf("%v", err)
		} else if unlocked {
			response.RelayState |= 0x01
		}

		if open, err := ut0311.state.DoorOpen(1); err != nil {
			return nil, err
		} else {
			response.Door1State = open
		}

		if button, err := ut0311.state.DoorButton(1); err != nil {
			return nil, err
		} else {
			response.Door1Button = button
		}

		// ... door 2
		if unlocked, err := ut0311.state.DoorUnlocked(2); err != nil {
			warnf("%v", err)
		} else if unlocked {
			response.RelayState |= 0x02
		}

		if open, err := ut0311.state.DoorOpen(2); err != nil {
			return nil, err
		} else {
			response.Door2State = open
		}

		if button, err := ut0311.state.DoorButton(2); err != nil {
			return nil, err
		} else {
			response.Door2Button = button
		}

		// ... door 3
		if unlocked, err := ut0311.state.DoorUnlocked(3); err != nil {
			warnf("%v", err)
		} else if unlocked {
			response.RelayState |= 0x04
		}

		if open, err := ut0311.state.DoorOpen(3); err != nil {
			return nil, err
		} else {
			response.Door3State = open
		}

		if button, err := ut0311.state.DoorButton(3); err != nil {
			return nil, err
		} else {
			response.Door3Button = button
		}

		// ... door 4
		if unlocked, err := ut0311.state.DoorUnlocked(4); err != nil {
			warnf("%v", err)
		} else if unlocked {
			response.RelayState |= 0x08
		}

		if open, err := ut0311.state.DoorOpen(4); err != nil {
			return nil, err
		} else {
			response.Door4State = open
		}

		if button, err := ut0311.state.DoorButton(4); err != nil {
			return nil, err
		} else {
			response.Door4Button = button
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
