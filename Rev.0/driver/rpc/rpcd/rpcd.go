package rpcd

import (
	"net"
	"net/http"
	"net/rpc"

	"ssmp/log"
	"ssmp/ssmp"
)

type RPCD struct {
	ssmp ssmp.SSMP
}

type KV struct {
	OID   string
	Value any
}

func (r *RPCD) Get(oid string, reply *any) error {
	debugf("get %v", oid)

	if v, err := r.ssmp.Get(oid); err != nil {
		return err
	} else {
		*reply = v
		return nil
	}
}

func (r *RPCD) Set(kv KV, reply *any) error {
	debugf("set %v %v", kv.OID, kv.Value)

	if v, err := r.ssmp.Set(kv.OID, kv.Value); err != nil {
		return err
	} else {
		*reply = v
		return nil
	}
}

func Run() {
	rpcd := RPCD{
		ssmp: ssmp.SSMP{},
	}

	rpc.Register(&rpcd)
	rpc.HandleHTTP()

	if l, err := net.Listen("tcp", ":1234"); err != nil {
		errorf("listen error:", err)
	} else {
		http.Serve(l, nil)
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
