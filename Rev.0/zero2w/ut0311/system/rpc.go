package system

import (
	"fmt"
	"net/rpc"
)

const service = "system"

var api = struct {
	getDoor string
}{
	getDoor: service + ".GetDoor",
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
