package system

import (
	"fmt"
	"net/rpc"
)

const service = "system"

var api = struct {
	setDoor         string
	getDoor         string
	getInterlock    string
	setInterlock    string
	getAntiPassback string
	setAntiPassback string
	getSwipe        string
	putSwipe        string
}{
	setDoor:         service + ".SetDoor",
	getDoor:         service + ".GetDoor",
	getInterlock:    service + ".GetInterlock",
	setInterlock:    service + ".SetInterlock",
	getAntiPassback: service + ".GetAntiPassback",
	setAntiPassback: service + ".SetAntiPassback",
	getSwipe:        service + ".GetSwipe",
	putSwipe:        service + ".PutSwipe",
}

func (s *System) GetDoor(controller uint32, door uint8) (DoorMode, uint8, error) {
	debugf("get-door %v %v", controller, door)

	var args = struct {
		Controller uint32
		Door       uint8
		Mode       uint8
		Delay      uint8
	}{
		Controller: controller,
		Door:       door,
	}

	var reply = struct {
		Controller uint32
		Door       uint8
		Mode       uint8
		Delay      uint8
	}{}

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return ModeUnknown, 5, err
	} else if err := client.Call(api.getDoor, args, &reply); err != nil {
		// FIXME use wrapped error ??
		println(">>>>>>>>>>>>>>>>> FIXME::use wrapped error")
		if fmt.Sprintf("%v", err) == "record not found" {
			return Controlled, 5, nil
		} else {
			return ModeUnknown, 5, err
		}
	} else if mode, ok := modes[reply.Mode]; !ok {
		return ModeUnknown, 5, fmt.Errorf("invalid door mode (%v)", reply.Mode)
	} else {
		return mode, reply.Delay, nil
	}
}

func (s *System) SetDoor(controller uint32, door uint8, mode DoorMode, delay uint8) (DoorMode, uint8, error) {
	debugf("set-door %v %v %v %v", controller, door, mode, delay)

	var args = struct {
		Controller uint32
		Door       uint8
		Mode       uint8
		Delay      uint8
	}{
		Controller: controller,
		Door:       door,
		Mode:       uint8(mode),
		Delay:      delay,
	}

	var reply = struct {
		Controller uint32
		Door       uint8
		Mode       uint8
		Delay      uint8
	}{}

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return ModeUnknown, 0, err
	} else if err := client.Call(api.setDoor, args, &reply); err != nil {
		return ModeUnknown, 0, err
	} else if m, ok := modes[reply.Mode]; !ok {
		return ModeUnknown, 0, fmt.Errorf("invalid door control mode (%v)", reply)
	} else {
		return m, reply.Delay, nil
	}
}

func (s *System) GetInterlock(controller uint32) (Interlock, error) {
	debugf("get-interlock %v", controller)

	var args = struct {
		Controller uint32
		Interlock  uint8
	}{
		Controller: controller,
	}

	var reply = struct {
		Controller uint32
		Interlock  uint8
	}{}

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return InterlockUnknown, err
	} else if err := client.Call(api.getInterlock, args, &reply); err != nil {
		return InterlockUnknown, err
	} else if v, ok := interlocks[reply.Interlock]; !ok {
		return InterlockUnknown, fmt.Errorf("invalid interlock (%v)", reply)
	} else {
		return v, nil
	}
}

func (s *System) SetInterlock(controller uint32, interlock Interlock) (Interlock, error) {
	debugf("set-interlock %v %v", controller, interlock)

	var args = struct {
		Controller uint32
		Interlock  uint8
	}{
		Controller: controller,
		Interlock:  uint8(interlock),
	}

	var reply = struct {
		Controller uint32
		Interlock  uint8
	}{}

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return InterlockUnknown, err
	} else if err := client.Call(api.setInterlock, args, &reply); err != nil {
		return InterlockUnknown, err
	} else if v, ok := interlocks[reply.Interlock]; !ok {
		return InterlockUnknown, fmt.Errorf("invalid interlock (%v)", reply)
	} else {
		return v, nil
	}
}

func (s *System) GetAntiPassback(controller uint32) (AntiPassback, error) {
	debugf("get-antipassback %v", controller)

	var args = struct {
		Controller   uint32
		AntiPassback uint8
	}{
		Controller: controller,
	}

	var reply = struct {
		Controller   uint32
		AntiPassback uint8
	}{}

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return AntiPassbackUnknown, err
	} else if err := client.Call(api.getAntiPassback, args, &reply); err != nil {
		return AntiPassbackUnknown, err
	} else if v, ok := antipassbacks[reply.AntiPassback]; !ok {
		return AntiPassbackUnknown, fmt.Errorf("invalid anti-passback (%v)", reply)
	} else {
		return v, nil
	}
}

func (s *System) SetAntiPassback(controller uint32, antipassback AntiPassback) (AntiPassback, error) {
	debugf("set-antipassback %v %v", controller, antipassback)

	var args = struct {
		Controller   uint32
		AntiPassback uint8
	}{
		Controller:   controller,
		AntiPassback: uint8(antipassback),
	}

	var reply = struct {
		Controller   uint32
		AntiPassback uint8
	}{}

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return AntiPassbackUnknown, err
	} else if err := client.Call(api.setAntiPassback, args, &reply); err != nil {
		return AntiPassbackUnknown, err
	} else if v, ok := antipassbacks[reply.AntiPassback]; !ok {
		return AntiPassbackUnknown, fmt.Errorf("invalid anti-passback (%v)", reply)
	} else {
		return v, nil
	}
}

func (s *System) GetSwipe(controller uint32, card uint32) (Swipe, error) {
	debugf("get-swipe %v %v", controller, card)

	var args = Swipe{
		Controller: controller,
		Card:       card,
	}

	var reply = Swipe{}

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return Swipe{}, err
	} else if err := client.Call(api.getSwipe, args, &reply); err != nil {
		return Swipe{}, err
	} else {
		return reply, nil
	}
}

func (s *System) PutSwipe(controller uint32, door uint8, card uint32) (bool, error) {
	debugf("put-swipe %v %v", controller, card)

	var args = Swipe{
		Controller: controller,
		Door:       door,
		Card:       card,
	}

	var reply bool

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return false, err
	} else if err := client.Call(api.putSwipe, args, &reply); err != nil {
		return false, err
	} else {
		return reply, nil
	}
}
