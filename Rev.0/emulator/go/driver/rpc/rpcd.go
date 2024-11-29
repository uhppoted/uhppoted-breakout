package rpcx

import (
	"net/rpc"

	"emulator/MIB/types"
	"emulator/log"
)

type RPC struct {
}

func (r RPC) Get(oid types.OID) (uint32, error) {
	debugf("get %v\n", oid)

	var reply uint32

	if client, err := rpc.DialHTTP("tcp", "127.0.0.1:1234"); err != nil {
		return 0, err
	} else if err := client.Call("RPCD.Get", "asdfas", &reply); err != nil {
		return 0, err
	} else {
		return reply, nil
	}
}

func debugf(format string, args ...any) {
	log.Debugf("RPC", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("RPC", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("RPC", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("RPC", format, args...)
}
