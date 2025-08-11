package main

import (
	"flag"
	"fmt"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"

	"eventd/db"
	"eventd/rpc"
)

type run struct {
	bind string
	DSN  string
}

func makeRun() (run, error) {
	r := run{
		bind: DEFAULT_BIND,
		DSN:  DEFAULT_DSN,
	}

	flag.StringVar(&r.bind, "bind", r.bind, "bind address (in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flag.StringVar(&r.DSN, "DSN", r.DSN, "DB DSN e.g. sqlite3:///var/uhppoted/breakout/db/sqlite3/ut0311.db")
	flag.Parse()

	if r.DSN == "" {
		return run{}, fmt.Errorf("missing --DSN arg")
	}

	return r, nil
}

func (r run) exec() error {
	if rpc, err := rpc.NewRPCD(r.bind); err != nil {
		return err
	} else if rpc == nil {
		return fmt.Errorf("invalid RPC (%v)", rpc)
	} else if err := db.Init(r.DSN); err != nil {
		return err
	} else {
		var wg sync.WaitGroup

		wg.Add(1)
		go func() {
			rpc.Run()
			wg.Done()
		}()

		terminated := make(chan struct{})
		interrupt := make(chan os.Signal, 1)

		signal.Notify(interrupt, syscall.SIGINT, syscall.SIGTERM)

		<-interrupt

		rpc.Stop()

		go func() {
			wg.Wait()
			terminated <- struct{}{}
		}()

		timeout := time.Duration(5000) * time.Millisecond

		select {
		case <-terminated:
			infof("terminated")
		case <-time.After(timeout):
			warnf("shutdown timeout")
		}

		return nil
	}
}
