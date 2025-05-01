package UT0311

import (
	"net/netip"

	"github.com/uhppoted/uhppote-core/messages"
	// "github.com/uhppoted/uhppote-core/types"

	"emulator/scmp"
)

func (ut0311 *UT0311) onEvent(event any) {
	debugf("event %v", event)

	if listener, err := scmp.Get[netip.AddrPort](ut0311.config, scmp.OID_CONTROLLER_EVENT_LISTENER); err == nil && listener.IsValid() {
		sendto(listener, ut0311.makeEvent(event))
	}
}

func (ut0311 *UT0311) makeEvent(event any) messages.Event {
	// index := s.Events.Add(event)
	// s.Save()
	//
	// utc := time.Now().UTC()
	// datetime := utc.Add(time.Duration(s.TimeOffset))
	evt := messages.Event{
		// SerialNumber: s.SerialNumber,
		// EventIndex:   index,
		// SystemError:  s.SystemError,
		// SystemDate:   types.SystemDate(datetime),
		// SystemTime:   types.SystemTime(datetime),
		// SequenceId:   s.SequenceId,
		// SpecialInfo:  s.SpecialInfo,
		// RelayState:   s.relays(),
		// InputState:   s.InputState,
		//
		// Door1State: s.Doors.IsOpen(1),
		// Door2State: s.Doors.IsOpen(2),
		// Door3State: s.Doors.IsOpen(3),
		// Door4State: s.Doors.IsOpen(4),
		//
		// Door1Button: s.Doors.IsButtonPressed(1),
		// Door2Button: s.Doors.IsButtonPressed(2),
		// Door3Button: s.Doors.IsButtonPressed(3),
		// Door4Button: s.Doors.IsButtonPressed(4),
		//
		// EventType:  event.Type,
		// Reason:     event.Reason,
		// Timestamp:  event.Timestamp,
		// CardNumber: event.Card,
		// Granted:    event.Granted,
		// Door:       event.Door,
		// Direction:  event.Direction,
	}

	return evt
}
