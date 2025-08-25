package UT0311

import (
	"github.com/uhppoted/uhppote-core/messages"
	lib "github.com/uhppoted/uhppote-core/types"

	"ut0311/entities"
	"ut0311/scmp"
)

func (ut0311 *UT0311) makeListenEvent(controller uint32, event entities.Event) messages.Event {
	evt := messages.Event{
		SerialNumber: lib.SerialNumber(controller),

		EventIndex:  event.Index,
		EventType:   uint8(event.Type),
		Reason:      uint8(event.Reason),
		Timestamp:   lib.DateTime(event.Timestamp),
		CardNumber:  event.Card,
		Granted:     event.Granted,
		Door:        event.Door,
		Direction:   event.Direction,
		RelayState:  0x00,
		InputState:  0x00,
		SystemError: 0x00,
		SpecialInfo: 0x00,
		SequenceId:  0,
	}

	// ... system date/time
	if v, err := ut0311.state.DateTime(); err != nil {
		warnf("%v", err)
	} else {
		evt.SystemDate = lib.SystemDate(v)
		evt.SystemTime = lib.SystemTime(v)
	}

	// ... system errors
	errs := []uint16{entities.ErrMemory, entities.ErrQueue, entities.ErrI2C, entities.ErrRX8900, entities.ErrU2, entities.ErrU3, entities.ErrU4}
	if v, err := ut0311.state.SystemError(errs...); err != nil {
		warnf("%v", err)
	} else if v {
		evt.SystemError |= 0x01
	}

	if v, err := ut0311.state.SystemError(entities.ErrWatchdog); err != nil {
		warnf("%v", err)
	} else if v {
		evt.SystemError |= 0x02
	}

	if v, err := ut0311.state.SystemError(entities.ErrUnknown); err != nil {
		warnf("%v", err)
	} else if v {
		evt.SystemError |= 0x08
	}

	// ... special info
	if v, err := ut0311.state.SpecialInfo(); err != nil {
		warnf("%v", err)
	} else {
		evt.SpecialInfo = v
	}

	// ... sequence no.
	if v, err := ut0311.state.SequenceNo(); err != nil {
		warnf("%v", err)
	} else {
		evt.SequenceId = v
	}

	// ... door locks
	if unlocked, err := ut0311.state.DoorUnlocked(1); err != nil {
		warnf("%v", err)
	} else if unlocked {
		evt.RelayState |= 0x01
	}

	if unlocked, err := ut0311.state.DoorUnlocked(2); err != nil {
		warnf("%v", err)
	} else if unlocked {
		evt.RelayState |= 0x02
	}

	if unlocked, err := ut0311.state.DoorUnlocked(3); err != nil {
		warnf("%v", err)
	} else if unlocked {
		evt.RelayState |= 0x04
	}

	if unlocked, err := ut0311.state.DoorUnlocked(4); err != nil {
		warnf("%v", err)
	} else if unlocked {
		evt.RelayState |= 0x08
	}

	// ... door states
	if open, err := ut0311.state.DoorOpen(1); err != nil {
		warnf("%v", err)
	} else {
		evt.Door1State = open
	}

	if open, err := ut0311.state.DoorOpen(2); err != nil {
		warnf("%v", err)
	} else {
		evt.Door2State = open
	}

	if open, err := ut0311.state.DoorOpen(3); err != nil {
		warnf("%v", err)
	} else {
		evt.Door3State = open
	}

	if open, err := ut0311.state.DoorOpen(4); err != nil {
		warnf("%v", err)
	} else {
		evt.Door4State = open
	}

	// ... door buttons
	if button, err := ut0311.state.DoorButton(1); err != nil {
		warnf("%v", err)
	} else {
		evt.Door1Button = button
	}

	if button, err := ut0311.state.DoorButton(2); err != nil {
		warnf("%v", err)
	} else {
		evt.Door2Button = button
	}

	if button, err := ut0311.state.DoorButton(3); err != nil {
		warnf("%v", err)
	} else {
		evt.Door3Button = button
	}

	if button, err := ut0311.state.DoorButton(4); err != nil {
		warnf("%v", err)
	} else {
		evt.Door4Button = button
	}

	// ... inputs
	if tampered, err := scmp.Get[bool](ut0311.breakout, scmp.OID_ALARMS_TAMPER_DETECT); err != nil {
		warnf("%v", err)
	} else if tampered {
		evt.InputState |= 0x01
	}

	if fire, err := scmp.Get[bool](ut0311.breakout, scmp.OID_ALARMS_FIRE_ALARM); err != nil {
		warnf("%v", err)
	} else if fire {
		evt.InputState |= 0x02
	}

	return evt
}
