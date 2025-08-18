package rpcd

import (
	"fmt"
	"net/rpc"

	"ut0311/scmp"
)

func (r RPC) UnlockDoor(door uint8) (bool, error) {
	switch door {
	case 1:
		return r.unlock(scmp.OID_DOORS_1_UNLOCKED)

	case 2:
		return r.unlock(scmp.OID_DOORS_2_UNLOCKED)

	case 3:
		return r.unlock(scmp.OID_DOORS_3_UNLOCKED)

	case 4:
		return r.unlock(scmp.OID_DOORS_4_UNLOCKED)
	}

	return false, fmt.Errorf("invalid door (%v)", door)

}

func (r RPC) unlock(oid scmp.OID) (bool, error) {
	debugf("unlock %v", oid)

	var kv = KV{
		OID:   fmt.Sprintf("%v", oid),
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
