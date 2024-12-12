package rpcd

import (
	"fmt"
	"net"
	"net/http"
	"net/rpc"
	"time"

	"ssmp/log"
)

type RPCD int

type KV struct {
	OID   string
	Value any
}

func (r *RPCD) Get(oid string, reply *any) error {
	debugf("get %v", oid)

	// ... controller ID
	if oid == ".1.3.6.1.4.1.65536.2.1" {
		*reply = uint32(405419896)

		return nil
	}

	// ... controller version
	if oid == ".1.3.6.1.4.1.65536.2.2" {
		*reply = uint16(0x1234)

		return nil
	}

	// ... controller release date
	if oid == ".1.3.6.1.4.1.65536.2.3" {
		*reply = "2024-01-15"

		return nil
	}

	// ... controller date/time
	if oid == ".1.3.6.1.4.1.65536.2.8" {
		*reply = time.Now().Format("2006-01-02 15:04:05")

		return nil
	}

	return fmt.Errorf("unknown OID %v", oid)
}

func (r *RPCD) Set(kv KV, reply *any) error {
	debugf("set %v %v", kv.OID, kv.Value)

	// ... controller date/time
	if kv.OID == ".1.3.6.1.4.1.65536.2.8" {
		*reply = time.Now().Format("2006-01-02 15:04:05")

		return nil
	}

	return fmt.Errorf("unknown OID %v", kv.OID)
}

func Run() {
	rpcd := new(RPCD)

	rpc.Register(rpcd)
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
