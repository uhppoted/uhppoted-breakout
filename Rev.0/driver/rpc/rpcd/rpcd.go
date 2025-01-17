package rpcd

import (
	"context"
	"fmt"
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
	network string
	addr    string
	ssmp    ssmp.SSMP
	ctx     context.Context
	cancel  context.CancelFunc
}

type KV struct {
	OID   string
	Value any
}

func NewRPCD(address string) (*RPCD, error) {
	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(address); len(matches) < 3 {
		return nil, fmt.Errorf("invalid bind address (%v)", address)
	} else {
		ctx, cancel := context.WithCancel(context.Background())

		v := RPCD{
			network: matches[1],
			addr:    matches[2],
			ssmp:    ssmp.SSMP{},

			ctx:    ctx,
			cancel: cancel,
		}

		return &v, nil
	}
}

func (r *RPCD) Run() {
	rpc.Register(r)
	rpc.HandleHTTP()

	if r.network == "unix" {
		folder := filepath.Dir(r.addr)
		if err := os.MkdirAll(folder, 0766); err != nil {
			errorf("listen error: %v", err)
		}

		os.Remove(r.addr)
	}

	if l, err := net.Listen(r.network, r.addr); err != nil {
		errorf("listen error: %v", err)
	} else {
		go func() {
			<-r.ctx.Done()
			l.Close()
		}()

		infof("listening %v %v", r.network, r.addr)
		http.Serve(l, nil)
	}
}

func (r *RPCD) Stop() {
	r.cancel()
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
