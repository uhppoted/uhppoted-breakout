package rpcd

import (
	"net"
	"net/http"
	"net/rpc"
	"os"
	"path/filepath"
	"regexp"

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

func Run(bind string) {
	rpcd := RPCD{
		ssmp: ssmp.SSMP{},
	}

	rpc.Register(&rpcd)
	rpc.HandleHTTP()

	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(bind); len(matches) < 3 {
		errorf("invalid bind address (%v)", bind)
	} else {
		network := matches[1]
		addr := matches[2]

		if network == "unix" {
			folder := filepath.Dir(addr)
			if err := os.MkdirAll(folder, 0766); err != nil {
				errorf("listen error: %v", err)
			}

			os.Remove(addr)
		}

		defer func() {
			if network == "unix" {
				if err := os.Remove(addr); err != nil {
					warnf("error removing Unix domain socket '%v' (%v)", addr, err)
				}
			}
		}()

		if l, err := net.Listen(network, addr); err != nil {
			errorf("listen error: %v", err)
		} else {
			infof("listening %v %v", network, addr)
			http.Serve(l, nil)
		}
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
