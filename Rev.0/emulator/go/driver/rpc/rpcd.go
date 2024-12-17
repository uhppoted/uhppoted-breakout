package rpcx

import (
	"fmt"
	"net/rpc"

	"emulator/MIB"
	"emulator/log"
)

type RPC struct {
}

type KV struct {
	OID   string
	Value any
}

func (r RPC) get(oid MIB.OID) (any, error) {
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

func (r RPC) GetUint8(oid MIB.OID) (uint8, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u8, ok := v.(uint8); !ok {
		return 0, fmt.Errorf("invalid uint8 value (%T)", v)
	} else {
		return u8, nil
	}
}

func (r RPC) GetUint16(oid MIB.OID) (uint16, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u16, ok := v.(uint16); !ok {
		return 0, fmt.Errorf("invalid uint16 value (%T)", v)
	} else {
		return u16, nil
	}
}

func (r RPC) GetUint32(oid MIB.OID) (uint32, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u32, ok := v.(uint32); !ok {
		return 0, fmt.Errorf("invalid uint32 value (%T)", v)
	} else {
		return u32, nil
	}
}

func (r RPC) GetBool(oid MIB.OID) (bool, error) {
	if v, err := r.get(oid); err != nil {
		return false, err
	} else if b, ok := v.(bool); !ok {
		return false, fmt.Errorf("invalid boolean value (%T)", v)
	} else {
		return b, nil
	}
}

func (r RPC) GetString(oid MIB.OID) (string, error) {
	if v, err := r.get(oid); err != nil {
		return "", err
	} else if s, ok := v.(string); !ok {
		return "", fmt.Errorf("invalid string value (%T)", v)
	} else {
		return s, nil
	}
}

func (r RPC) Set(oid MIB.OID, value any) (any, error) {
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
