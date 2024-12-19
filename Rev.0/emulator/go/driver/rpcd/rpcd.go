package rpcd

import (
	"fmt"
	"net/rpc"

	"emulator/log"
	"emulator/scmp"
)

type RPC struct {
}

type KV struct {
	OID   string
	Value any
}

func (r RPC) get(oid scmp.OID) (any, error) {
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

func (r RPC) GetUint8(oid scmp.OID) (uint8, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u8, ok := v.(uint8); !ok {
		return 0, fmt.Errorf("invalid uint8 value (%T)", v)
	} else {
		return u8, nil
	}
}

func (r RPC) GetUint16(oid scmp.OID) (uint16, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u16, ok := v.(uint16); !ok {
		return 0, fmt.Errorf("invalid uint16 value (%T)", v)
	} else {
		return u16, nil
	}
}

func (r RPC) GetUint32(oid scmp.OID) (uint32, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u32, ok := v.(uint32); !ok {
		return 0, fmt.Errorf("invalid uint32 value (%T)", v)
	} else {
		return u32, nil
	}
}

func (r RPC) GetBool(oid scmp.OID) (bool, error) {
	if v, err := r.get(oid); err != nil {
		return false, err
	} else if b, ok := v.(bool); !ok {
		return false, fmt.Errorf("invalid boolean value (%T)", v)
	} else {
		return b, nil
	}
}

func (r RPC) GetString(oid scmp.OID) (string, error) {
	if v, err := r.get(oid); err != nil {
		return "", err
	} else if s, ok := v.(string); !ok {
		return "", fmt.Errorf("invalid string value (%T)", v)
	} else {
		return s, nil
	}
}

func (r RPC) GetOctets(oid scmp.OID) ([]byte, error) {
	if v, err := r.get(oid); err != nil {
		return nil, err
	} else if bytes, ok := v.([]byte); !ok {
		return nil, fmt.Errorf("invalid octets value (%T)", v)
	} else {
		return bytes, nil
	}
}

func (r RPC) SetUint8(oid scmp.OID, value uint8) (uint8, error) {
	debugf("get %v %v", oid, value)

	var kv = KV{
		OID:   fmt.Sprintf("%v", oid),
		Value: value,
	}

	var reply any

	if client, err := rpc.DialHTTP("tcp", "127.0.0.1:1234"); err != nil {
		return 0, err
	} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
		return 0, err
	} else if u8, ok := reply.(uint8); !ok {
		return 0, fmt.Errorf("invalid reply - expected 'uint8', got '%T'", reply)
	} else {
		return u8, nil
	}
}

func (r RPC) SetString(oid scmp.OID, value string) (string, error) {
	debugf("get %v %v", oid, value)

	var kv = KV{
		OID:   fmt.Sprintf("%v", oid),
		Value: value,
	}

	var reply any

	if client, err := rpc.DialHTTP("tcp", "127.0.0.1:1234"); err != nil {
		return "", err
	} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
		return "", err
	} else if s, ok := reply.(string); !ok {
		return "", fmt.Errorf("invalid reply - expected 'string', got '%T'", reply)
	} else {
		return s, nil
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
