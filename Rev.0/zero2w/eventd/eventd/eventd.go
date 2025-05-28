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

	"eventd/db"
	"eventd/entities"
	"eventd/log"
)

const LOGTAG = "eventd"

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

func (d *EventD) Add(event entities.Event, reply *uint32) error {
	debugf("add %v", event)

	if index, err := db.PutEvent(event); err != nil {
		return err
	} else {
		*reply = index

		return nil
	}

	return fmt.Errorf("** NOT IMPLEMENTED **")
}

func debugf(format string, args ...any) {
	log.Debugf(LOGTAG, format, args...)
}

func infof(format string, args ...any) {
	log.Infof(LOGTAG, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(LOGTAG, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(LOGTAG, format, args...)
}
