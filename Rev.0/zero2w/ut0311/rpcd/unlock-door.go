package rpcd

import (
	"fmt"
	"net/rpc"
)

func (r RPC) UnlockDoor(door uint8) (bool, error) {
	debugf("unlock %v", door)

	if door < 1 || door > 4 {
		return false, fmt.Errorf("invalid door (%v)", door)
	} else {
		var kv = TV{
			Tag:   fmt.Sprintf("controller.door.%v.unlocked", door),
			Value: true,
		}

		var reply any

		if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
			return false, err
		} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
			return false, err
		} else if unlocked, ok := reply.(bool); !ok {
			return unlocked, fmt.Errorf("unrecognized type (%T)", reply)
		} else {
			return unlocked, nil
		}
	}

}
