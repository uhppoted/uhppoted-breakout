package UT0311

import (
	"net/netip"

	"github.com/uhppoted/uhppote-core/messages"
	lib "github.com/uhppoted/uhppote-core/types"

	"emulator/driver/rpcd"
	"emulator/entities"
	"emulator/events"
	"emulator/scmp"
)

func (ut0311 *UT0311) onEvent(event any) {
	debugf("event %T %v", event, event)

	if v, ok := event.(rpcd.Event); ok {
		controller := v.ID
		evt := entities.Event{
			Index:     0,
			Type:      events.Lookup(v.Var.OID),
			Granted:   false,
			Door:      events.Door(v.Var.OID),
			Direction: 0,
			Card:      0,
			Timestamp: v.Timestamp,
			Reason:    events.Reason(v.Var.OID, v.Var.Value),
		}

		if index, err := ut0311.events.Add(controller, evt); err != nil {
			warnf("%v", err)
		} else {
			evt.Index = index

			if listener, err := scmp.Get[netip.AddrPort](ut0311.config, scmp.OID_CONTROLLER_EVENT_LISTENER); err == nil && listener.IsValid() {
				e := ut0311.makeListenEvent(v.ID, evt)
				ut0311.udp.sendto(listener, e)
			}
		}
	}
}

func (ut0311 *UT0311) makeListenEvent(controller uint32, event entities.Event) messages.Event {
	evt := messages.Event{
		SerialNumber: lib.SerialNumber(controller),

		EventIndex: event.Index,
		EventType:  uint8(event.Type),
		Reason:     uint8(event.Reason),
		Timestamp:  lib.DateTime(event.Timestamp),
		CardNumber: event.Card,
		Granted:    event.Granted,
		Door:       event.Door,
		Direction:  event.Direction,
	}

	// ... system date/time
	if datetime, err := scmp.Get[lib.DateTime](ut0311.breakout, scmp.OID_CONTROLLER_DATETIME); err != nil {
		warnf("%v", err)
	} else {
		evt.SystemDate = lib.SystemDate(datetime)
		evt.SystemTime = lib.SystemTime(datetime)
	}

	// ... system errors
	if v, err := scmp.Get[uint16](ut0311.breakout, scmp.OID_CONTROLLER_SYSERR); err != nil {
		warnf("%v", err)
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

		evt.SystemError = syserr
	}

	// ... special info
	if v, err := scmp.Get[uint8](ut0311.breakout, scmp.OID_CONTROLLER_SYSINFO); err != nil {
		warnf("%v", err)
	} else {
		evt.SpecialInfo = v
	}

	// ... sequence no.
	if v, err := scmp.Get[uint32](ut0311.system, scmp.OID_CONTROLLER_SEQUENCE_NUMBER); err != nil {
		warnf("%v", err)
	} else {
		evt.SequenceId = v
	}

	// ... door locks
	if locked, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_1_UNLOCKED); err != nil {
		warnf("%v", err)
	} else if !locked {
		evt.RelayState |= 0x01
	}

	if locked, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_2_UNLOCKED); err != nil {
		warnf("%v", err)
	} else if !locked {
		evt.RelayState |= 0x02
	}

	if locked, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_3_UNLOCKED); err != nil {
		warnf("%v", err)
	} else if !locked {
		evt.RelayState |= 0x04
	}

	if locked, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_4_UNLOCKED); err != nil {
		warnf("%v", err)
	} else if !locked {
		evt.RelayState |= 0x08
	}

	// ... door states
	if open, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_1_OPEN); err != nil {
		warnf("%v", err)
	} else {
		evt.Door1State = open
	}

	if open, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_2_OPEN); err != nil {
		warnf("%v", err)
	} else {
		evt.Door2State = open
	}

	if open, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_3_OPEN); err != nil {
		warnf("%v", err)
	} else {
		evt.Door3State = open
	}

	if open, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_4_OPEN); err != nil {
		warnf("%v", err)
	} else {
		evt.Door4State = open
	}

	// ... door buttons
	if pressed, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_1_BUTTON); err != nil {
		warnf("%v", err)
	} else {
		evt.Door1Button = pressed
	}

	if pressed, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_2_BUTTON); err != nil {
		warnf("%v", err)
	} else {
		evt.Door2Button = pressed
	}

	if pressed, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_3_BUTTON); err != nil {
		warnf("%v", err)
	} else {
		evt.Door3Button = pressed
	}

	if pressed, err := scmp.Get[bool](ut0311.breakout, scmp.OID_DOORS_4_BUTTON); err != nil {
		warnf("%v", err)
	} else {
		evt.Door4Button = pressed
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
