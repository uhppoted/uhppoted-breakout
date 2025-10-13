package rpcd

import (
	"fmt"
	"net/rpc"
)

func (r RPC) Denied(controller uint32, door uint8) error {
	debugf("denied %v %v", controller, door)

	if door < 1 || door > 4 {
		return fmt.Errorf("invalid door (%v)", door)
	} else {
		var kv = TV{
			Tag:   fmt.Sprintf("controller.door.%v.access", door),
			Value: uint8(255),
		}

		var reply any

		if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
			return err
		} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
			return err
		} else {
			return nil
		}
	}
}

func (r RPC) Granted(controller uint32, door uint8) error {
	debugf("granted %v %v", controller, door)

	if door < 1 || door > 4 {
		return fmt.Errorf("invalid door (%v)", door)
	} else {
		var kv = TV{
			Tag:   fmt.Sprintf("controller.door.%v.access", door),
			Value: uint8(1),
		}

		var reply any

		if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
			return err
		} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
			return err
		} else {
			return nil
		}
	}
}
