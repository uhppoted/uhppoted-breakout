package UT0311

import (
	"net/netip"

	"emulator/driver/rpcd"
	"emulator/entities"
	"emulator/events"
	"emulator/scmp"
)

func (ut0311 *UT0311) onTrap(event any) {
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
