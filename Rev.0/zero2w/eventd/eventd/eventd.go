package eventd

import (
	"context"
	"fmt"
	"net"
	"net/http"
	"net/rpc"
	"os"
	"path/filepath"
	"regexp"

	"eventd/log"
)

type EventD struct {
	ctx    context.Context
	cancel context.CancelFunc

	listen struct {
		network string
		addr    string
	}
}

func NewEventD(listen string) (*EventD, error) {
	ctx, cancel := context.WithCancel(context.Background())

	v := EventD{
		ctx:    ctx,
		cancel: cancel,
	}

	// ... set 'listen' address:port
	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(listen); len(matches) < 3 {
		return nil, fmt.Errorf("invalid bind address (%v)", listen)
	} else {
		v.listen = struct {
			network string
			addr    string
		}{
			network: matches[1],
			addr:    matches[2],
		}
	}

	return &v, nil
}

func (d *EventD) Run() {
	rpc.Register(d)
	rpc.HandleHTTP()

	if d.listen.network == "unix" {
		folder := filepath.Dir(d.listen.addr)
		if err := os.MkdirAll(folder, 0766); err != nil {
			errorf("listen error: %v", err)
		}

		os.Remove(d.listen.addr)
	}

	if l, err := net.Listen(d.listen.network, d.listen.addr); err != nil {
		errorf("listen error: %v", err)
	} else {
		go func() {
			<-d.ctx.Done()
			l.Close()
		}()

		infof("listening %v %v", d.listen.network, d.listen.addr)
		http.Serve(l, nil)
	}
}

func (d *EventD) Stop() {
	d.cancel()
}

func (d *EventD) Get(oid string, reply *any) error {
	debugf("get %v", oid)

	// if v, err := r.ssmp.Get(oid); err != nil {
	//     return err
	// } else {
	//     *reply = v
	//     return nil
	// }

	return fmt.Errorf("** NOT IMPLEMENTED **")
}

// func (r *RPCD) Set(kv KV, reply *any) error {
//     debugf("set %v %v", kv.OID, kv.Value)
//
//     if v, err := r.ssmp.Set(kv.OID, kv.Value); err != nil {
//         return err
//     } else {
//         *reply = v
//         return nil
//     }
// }

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
