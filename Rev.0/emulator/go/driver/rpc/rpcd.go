package rpcx

import (
	"fmt"
	"net/rpc"

	"emulator/MIB/types"
	"emulator/log"
)

type RPC struct {
}

type KV struct {
	OID   string
	Value any
}

func (r RPC) Get(oid types.OID) (any, error) {
	debugf("get %v", oid)

	var key = fmt.Sprintf("%v", oid)
	var reply any

	if client, err := rpc.DialHTTP("tcp", "127.0.0.1:1234"); err != nil {
		return 0, err
	} else if err := client.Call("RPCD.Get", key, &reply); err != nil {
		return 0, err
	} else {
		return reply, nil
	}
}

func (r RPC) Set(oid types.OID, value any) (any, error) {
	debugf("get %v %v", oid, value)

	var kv = KV{
		OID:   fmt.Sprintf("%v", oid),
		Value: value,
	}

	var reply any

	if client, err := rpc.DialHTTP("tcp", "127.0.0.1:1234"); err != nil {
		return nil, err
	} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
		return nil, err
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
