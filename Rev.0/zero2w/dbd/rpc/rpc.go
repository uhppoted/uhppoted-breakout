package rpc

import (
	"context"
	"fmt"
	"net"
	"net/http"
	lib "net/rpc"
	"os"
	"path/filepath"
	"regexp"

	"dbd/cards"
	"dbd/events"
	"dbd/log"
	"dbd/system"
)

const LOGTAG = "rpc"

type rpcd struct {
	ctx    context.Context
	cancel context.CancelFunc

	system system.System
	cards  cards.Cards
	events events.Events

	listen struct {
		network string
		addr    string
	}
}

func NewRPCD(listen string) (*rpcd, error) {
	ctx, cancel := context.WithCancel(context.Background())

	v := rpcd{
		ctx:    ctx,
		cancel: cancel,

		system: system.System{},
		cards:  cards.Cards{},
		events: events.Events{},
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

func (r *rpcd) Run() {
	lib.RegisterName("system", &r.system)
	lib.RegisterName("cards", &r.cards)
	lib.RegisterName("events", &r.events)

	lib.HandleHTTP()

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

func (r *rpcd) Stop() {
	r.cancel()
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
