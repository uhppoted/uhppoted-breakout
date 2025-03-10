package main

import (
	"flag"
	"fmt"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"

	"ssmp/log"
	"ssmp/rpcd"
)

const VERSION = "v0.0.0"

var options = struct {
	device string
	bind   string
}{
	device: "",
	bind:   "tcp:::1234",
}

func main() {
	fmt.Printf("SSMP RPC DRIVER %v\n", VERSION)

	flag.StringVar(&options.device, "device", options.device, "serial device ID")
	flag.StringVar(&options.bind, "bind", options.bind, "bind address (in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flag.Parse()

	if options.device == "" {
		errorf("missing --device arg")
		os.Exit(1)
	} else if r, err := rpcd.NewRPCD(options.device, options.bind); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else if r == nil {
		errorf("invalid RPCD (%v)", r)
		os.Exit(1)
	} else {
		var wg sync.WaitGroup

		wg.Add(1)
		go func() {
			r.Run()
			wg.Done()
		}()

		terminated := make(chan struct{})
		interrupt := make(chan os.Signal, 1)

		signal.Notify(interrupt, syscall.SIGINT, syscall.SIGTERM)

		<-interrupt

		r.Stop()

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
	}
}

func infof(format string, args ...any) {
	log.Infof("RPCD", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("RPCD", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("RPCD", format, args...)
}
