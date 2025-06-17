package main

import (
	"flag"
	"fmt"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"

	"ssmp/rpcd"
)

type run struct {
	device string
	bind   string
	dial   string
}

func makeRun() (run, error) {
	r := run{
		device: DEFAULT_DEVICE,
		bind:   DEFAULT_BIND,
		dial:   DEFAULT_DIAL,
	}

	flag.StringVar(&r.device, "device", r.device, "serial device ID")
	flag.StringVar(&r.bind, "bind", r.bind, "bind address (in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flag.StringVar(&r.dial, "dial", r.dial, "dial address (for events, in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flag.Parse()

	if r.device == "" {
		return run{}, fmt.Errorf("missing --device arg")
	}

	return r, nil
}

func (r run) exec() error {
	if rpc, err := rpcd.NewRPCD(r.device, r.bind, r.dial); err != nil {
		return err
	} else if rpc == nil {
		return fmt.Errorf("invalid RPCD (%v)", rpc)
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
