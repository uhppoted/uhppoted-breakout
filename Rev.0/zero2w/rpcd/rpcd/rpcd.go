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
	"time"

	"ssmp/encoding/ASN.1"
	"ssmp/log"
	"ssmp/ssmp"
)

type RPCD struct {
	ssmp   *ssmp.SSMP
	ctx    context.Context
	cancel context.CancelFunc

	listen struct {
		network string
		addr    string
	}

	dial struct {
		network string
		addr    string
	}
}

type KV struct {
	OID   string
	Value any
}

type Event struct {
	Timestamp time.Time
	ID        uint32
	Category  uint32
	Event     uint32
	OID       string
	Value     any
}

func NewRPCD(deviceId string, listen string, dial string) (*RPCD, error) {
	ctx, cancel := context.WithCancel(context.Background())

	v := RPCD{
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

	// ... set 'dial' address:port
	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(dial); len(matches) < 3 {
		return nil, fmt.Errorf("invalid dial address (%v)", dial)
	} else {
		v.dial = struct {
			network string
			addr    string
		}{
			network: matches[1],
			addr:    matches[2],
		}
	}

	// ... create SSMP codec
	if d, err := ssmp.NewSSMP(deviceId, v.Trap); err != nil {
		return nil, err
	} else {
		v.ssmp = d
	}

	return &v, nil
}

func (r *RPCD) Run() {
	go func() {
		r.ssmp.Run()
	}()

	rpc.Register(r)
	rpc.HandleHTTP()

	if r.listen.network == "unix" {
		folder := filepath.Dir(r.listen.addr)
		if err := os.MkdirAll(folder, 0766); err != nil {
			errorf("listen error: %v", err)
		}

		os.Remove(r.listen.addr)
	}

	if l, err := net.Listen(r.listen.network, r.listen.addr); err != nil {
		errorf("listen error: %v", err)
	} else {
		go func() {
			<-r.ctx.Done()
			l.Close()
		}()

		infof("listening %v %v", r.listen.network, r.listen.addr)
		http.Serve(l, nil)
	}
}

func (r *RPCD) Stop() {
	r.cancel()
	r.ssmp.Stop()
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

func (r *RPCD) Trap(trap any) {
	debugf("trap %#v", trap)

	if v, ok := trap.(BER.Trap); ok {
		timestamp := time.Now().Round(time.Second)
		if t, err := time.ParseInLocation("2006-01-02 15:04:05", v.Timestamp, time.Local); err == nil {
			timestamp = t
		}

		oid := fmt.Sprintf("%v", v.OID)

		var event = Event{
			Timestamp: timestamp,
			ID:        v.ID,
			Category:  v.Category,
			Event:     v.Event,
			OID:       oid,
			Value:     nil, // FIXME
		}

		var reply any

		if client, err := rpc.DialHTTP(r.dial.network, r.dial.addr); err != nil {
			errorf("trap %v", err)
		} else if err := client.Call("RPC.Trap", event, &reply); err != nil {
			errorf("trap %v", err)
		} else {
			infof("trap sent (%v)", reply)
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
