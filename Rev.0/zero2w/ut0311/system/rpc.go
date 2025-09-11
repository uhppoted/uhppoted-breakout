package system

import (
	"fmt"
	"net/rpc"
)

const service = "system"

var api = struct {
	setInterlock string
	getDoor      string
}{
	setInterlock: service + ".SetInterlock",
	getDoor:      service + ".GetDoor",
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

	var reply uint8

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return Unknown, err
	} else if err := client.Call(api.setInterlock, args, &reply); err != nil {
		return Unknown, err
	} else if v, ok := interlocks[reply]; !ok {
		return Unknown, fmt.Errorf("invalid interlock (%v)", reply)
	} else {
		return v, nil
	}
}

func (s *System) GetDoor(controller uint32, door uint8) (DoorMode, uint8, error) {
	debugf("get-door %v %v", controller, door)

	var args = struct {
		Controller uint32
		Door       uint8
	}{
		Controller: controller,
		Door:       door,
	}

	var reply = struct {
		Mode  uint8
		Delay uint8
	}{}

	if client, err := rpc.DialHTTP(s.dial.network, s.dial.address); err != nil {
		return 0, 0, err
	} else if err := client.Call(api.getDoor, args, &reply); err != nil {
		return 0, 0, err
	} else if mode, ok := modes[reply.Mode]; !ok {
		return 0, 0, fmt.Errorf("invalid door mode (%v)", reply.Mode)
	} else {
		return mode, reply.Delay, nil
	}
}
