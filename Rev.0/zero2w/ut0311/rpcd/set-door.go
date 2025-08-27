package rpcd

import (
	"fmt"
	"net/rpc"
)

type DoorMode uint8

const (
	NormallyOpen   DoorMode = 1
	NormallyClosed DoorMode = 2
	Controlled     DoorMode = 3
)

func (m DoorMode) String() string {
	switch m {
	case NormallyOpen:
		return "normally-open"

	case NormallyClosed:
		return "normally-closed"

	case Controlled:
		return "controlled"

	default:
		return "unknown"
	}
}

func (r RPC) SetDoorMode(controller uint32, door uint8, mode DoorMode) (uint8, error) {
	debugf("set-door-mode %v %v %v", controller, door, mode)

	if door < 1 || door > 4 {
		return 0, fmt.Errorf("invalid door (%v)", door)
	} else if mode != NormallyOpen && mode != NormallyClosed && mode != Controlled {
		return 0, fmt.Errorf("invalid door mode (%v, %v)", door, mode)
	} else {
		var kv = TV{
			Tag:   fmt.Sprintf("controller.door.%v.mode", door),
			Value: uint8(mode),
		}

		var reply any

		if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
			return 0, err
		} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
			return 0, err
		} else {
			switch v := reply.(type) {
			case uint8:
				return v, nil

			case int64:
				return uint8(v), nil

			default:
				return 0, fmt.Errorf("unrecognized type (%T)", reply)
			}
		}
	}
}

func (r RPC) SetDoorDelay(controller uint32, door uint8, delay uint8) (uint8, error) {
	debugf("set-door-delay %v %v %v", controller, door, delay)

	if door < 1 || door > 4 {
		return 0, fmt.Errorf("invalid door (%v)", door)
	} else if delay < 1 || delay > 255 {
		return 0, fmt.Errorf("invalid door delay (%v, %v)", door, delay)
	} else {
		var kv = TV{
			Tag:   fmt.Sprintf("controller.door.%v.delay", door),
			Value: delay,
		}

		var reply any

		if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
			return 0, err
		} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
			return 0, err
		} else {
			switch v := reply.(type) {
			case uint8:
				return v, nil

			case int64:
				return uint8(v), nil

			default:
				return 0, fmt.Errorf("unrecognized type (%T)", reply)
			}
		}
	}
}
