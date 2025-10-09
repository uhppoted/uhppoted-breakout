package rpcd

import (
	"fmt"
	"net/rpc"

	"github.com/uhppoted/uhppote-core/types"
)

func (r RPC) SetTime(controller uint32, datetime types.DateTime) (types.DateTime, error) {
	debugf("set-time %v %v", controller, datetime)

	var kv = TV{
		Tag:   "controller.system.datetime",
		Value: fmt.Sprintf("%v", datetime),
	}

	var reply any

	if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
		return types.DateTime{}, err
	} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
		return types.DateTime{}, err
	} else {
		switch v := reply.(type) {
		case string:
			return types.ParseDateTime(v)

		default:
			return types.DateTime{}, fmt.Errorf("unrecognized type (%T)", reply)
		}
	}
}
