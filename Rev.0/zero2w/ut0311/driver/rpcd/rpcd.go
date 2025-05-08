package rpcd

import (
	"fmt"
	"net"
	"net/http"
	"net/rpc"
	"os"
	"path/filepath"
	"regexp"
	"time"

	"emulator/log"
	"emulator/scmp"
)

type RPC struct {
	dial struct {
		network string
		address string
	}

	listen struct {
		network string
		address string
	}

	onEvent func(event any)
}

type KV struct {
	OID   string
	Value any
}

type Event struct {
	Timestamp time.Time
	ID        uint32
	Var       struct {
		OID   string
		Value any
	}
}

func NewRPC(dial string, listen string, onEvent func(event any)) (*RPC, error) {
	infof("init dial:%v", dial)

	r := RPC{
		onEvent: onEvent,
	}

	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(dial); len(matches) < 3 {
		return nil, fmt.Errorf("invalid RPC 'dial' address (%v)", dial)
	} else {
		r.dial.network = matches[1]
		r.dial.address = matches[2]
	}

	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(listen); len(matches) < 3 {
		return nil, fmt.Errorf("invalid RPC 'listen' address (%v)", dial)
	} else {
		r.listen.network = matches[1]
		r.listen.address = matches[2]
	}

	return &r, nil
}

func (r *RPC) Listen() {
	rpc.Register(r)
	rpc.HandleHTTP()

	if r.listen.network == "unix" {
		folder := filepath.Dir(r.listen.address)
		if err := os.MkdirAll(folder, 0766); err != nil {
			errorf("listen error: %v", err)
		}

		os.Remove(r.listen.address)
	}

	if l, err := net.Listen(r.listen.network, r.listen.address); err != nil {
		errorf("listen error: %v", err)
	} else {
		defer l.Close()
		// go func() {
		// 	<-r.ctx.Done()
		// 	l.Close()
		// }()

		infof("listening %v %v", r.listen.network, r.listen.address)
		http.Serve(l, nil)
	}
}

func (r RPC) get(oid scmp.OID) (any, error) {
	debugf("get %v", oid)

	var key = fmt.Sprintf("%v", oid)
	var reply any

	if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
		return 0, err
	} else if err := client.Call("RPCD.Get", key, &reply); err != nil {
		return 0, err
	} else {
		return reply, nil
	}
}

func (r RPC) set(oid scmp.OID, value any) (any, error) {
	debugf("set %v", oid)

	var kv = KV{
		OID:   fmt.Sprintf("%v", oid),
		Value: value,
	}
	var reply any

	if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
		return 0, err
	} else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
		return 0, err
	} else {
		return reply, nil
	}
}

func (r *RPC) Trap(event Event, reply *any) error {
	debugf("trap %v", event)

	if r.onEvent != nil {
		go func() {
			r.onEvent(event)
		}()
	}

	*reply = true

	return nil
}

func (r RPC) GetUint8(oid scmp.OID) (uint8, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u8, ok := v.(uint8); ok {
		return u8, nil
	} else if i64, ok := v.(int64); ok {
		return uint8(i64), nil
	} else {
		return 0, fmt.Errorf("invalid uint8 value (%T)", v)
	}
}

func (r RPC) GetUint16(oid scmp.OID) (uint16, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u16, ok := v.(uint16); ok {
		return u16, nil
	} else if i64, ok := v.(int64); ok {
		return uint16(i64), nil
	} else {
		return 0, fmt.Errorf("invalid uint16 value (%T)", v)
	}
}

func (r RPC) GetUint32(oid scmp.OID) (uint32, error) {
	if v, err := r.get(oid); err != nil {
		return 0, err
	} else if u32, ok := v.(uint32); ok {
		return u32, nil
	} else if i64, ok := v.(int64); ok {
		return uint32(i64), nil
	} else {
		return 0, fmt.Errorf("invalid uint32 value (%T)", v)
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
	debugf("set %v %v", oid, value)

	if reply, err := r.set(oid, value); err != nil {
		return 0, err
	} else if u8, ok := reply.(uint8); ok {
		return u8, nil
	} else if i64, ok := reply.(int64); ok {
		return uint8(i64), nil
	} else {
		return 0, fmt.Errorf("invalid reply - expected 'uint8', got '%T'", reply)
	}
}

func (r RPC) SetUint32(oid scmp.OID, value uint32) (uint32, error) {
	debugf("set %v %v", oid, value)

	if reply, err := r.set(oid, value); err != nil {
		return 0, err
	} else if u32, ok := reply.(uint32); !ok {
		return 0, fmt.Errorf("invalid reply - expected 'uint32', got '%T'", reply)
	} else {
		return u32, nil
	}
}

func (r RPC) SetBool(oid scmp.OID, value bool) (bool, error) {
	debugf("set %v %v", oid, value)

	if reply, err := r.set(oid, value); err != nil {
		return false, err
	} else if b, ok := reply.(bool); !ok {
		return false, fmt.Errorf("invalid reply - expected 'bool', got '%T'", reply)
	} else {
		return b, nil
	}
}

func (r RPC) SetString(oid scmp.OID, value string) (string, error) {
	debugf("set %v %v", oid, value)

	if reply, err := r.set(oid, value); err != nil {
		return "", err
	} else if v, ok := reply.(string); !ok {
		return "", fmt.Errorf("invalid reply - expected 'string', got '%T'", reply)
	} else {
		return v, nil
	}
}

func (r RPC) SetUint32A(oid scmp.OID, value []uint32) ([]uint32, error) {
	debugf("set %v %v", oid, value)

	if reply, err := r.set(oid, value); err != nil {
		return []uint32{}, err
	} else if v, ok := reply.([]uint32); !ok {
		return []uint32{}, fmt.Errorf("invalid reply - expected 'uint8', got '%T'", reply)
	} else {
		return v, nil
	}
}

func (r RPC) SetIndexedRecord(oid scmp.OID, index uint32, value any) (any, error) {
	debugf("set-indexed %v %v %v", oid, index, value)

	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (r RPC) DeleteIndexedRecord(oid scmp.OID, index uint32) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
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
