package rpcd

import (
	"fmt"
	"net/rpc"
)

type Interlock uint8

const (
	NoInterlock    Interlock = 0x00 // disabled
	Interlock12    Interlock = 0x01 // doors (1,2)
	Interlock34    Interlock = 0x02 // doors (3,4)
	Interlock12_34 Interlock = 0x03 // doors (1,2) and (3,4)
	Interlock123   Interlock = 0x04 // doors (1,2,3)
	Interlock1234  Interlock = 0x08 // doors (1,2,3,4)
)

func (i Interlock) String() string {
	switch i {
	case NoInterlock:
		return "disabled"

	case Interlock12:
		return "doors (1,2)"

	case Interlock34:
		return "doors (3,4)"

	case Interlock12_34:
		return "doors (1,2) and (3,4)"

	case Interlock123:
		return "doors (1,2,3)"

	case Interlock1234:
		return "doors (1,2,3,4)"

	default:
		return "unknown"
	}
}

func (r RPC) SetInterlock(controller uint32, interlock Interlock) (Interlock, error) {
	debugf("set-interlock %v %v", controller, interlock)

	if interlock != NoInterlock && interlock != Interlock12 && interlock != Interlock34 && interlock != Interlock12_34 && interlock != Interlock123 && interlock != Interlock1234 {
		return 0, fmt.Errorf("invalid door interlock (%v)", interlock)
	} else {
		var kv = TV{
			Tag:   fmt.Sprintf("controller.doors.interlock"),
			Value: uint8(interlock),
		}

		var reply any

		if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
			return 0, err
		} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
			return 0, err
		} else {
			switch v := reply.(type) {
			case uint8:
				return Interlock(v), nil

			case int64:
				return Interlock(v), nil

			default:
				return 0, fmt.Errorf("unrecognized type (%T)", reply)
			}
		}
	}
}
