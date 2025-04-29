package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"

	"ssmp/cache"
	"ssmp/log"
	"ssmp/rpcd"
)

const VERSION = "v0.0.0"

var options = struct {
	device    string
	bind      string
	dial      string
	cacheable string
}{
	device: "",
	bind:   "tcp:::1234",
	dial:   "tcp:::4321",
}

func main() {
	fmt.Printf("SSMP RPC DRIVER %v\n", VERSION)

	flag.StringVar(&options.device, "device", options.device, "serial device ID")
	flag.StringVar(&options.bind, "bind", options.bind, "bind address (in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flag.StringVar(&options.dial, "dial", options.dial, "dial address (for events, in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flag.StringVar(&options.cacheable, "cacheable", options.cacheable, "(optional) JSON file of cacheable OIDs and lifetimes")
	flag.Parse()

	if options.device == "" {
		errorf("missing --device arg")
		os.Exit(1)
	} else {
		if options.cacheable != "" {
			if bytes, err := os.ReadFile(options.cacheable); err != nil {
				errorf("missing 'cacheable' list (%v)", err)
			} else if cacheable, err := unmarshal(bytes); err != nil {
				errorf("bad 'cacheable' list (%v)", err)
			} else {
				cache.Init(cacheable)
			}
		}

		if r, err := rpcd.NewRPCD(options.device, options.bind, options.dial); err != nil {
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
}

func unmarshal(bytes []byte) (map[string]time.Duration, error) {
	m := map[string]string{}

	if err := json.Unmarshal(bytes, &m); err != nil {
		return nil, err
	} else {
		cacheable := map[string]time.Duration{}

		for k, v := range m {
			if d, err := time.ParseDuration(v); err != nil {
				return cacheable, err
			} else {
				cacheable[k] = d
			}
		}

		return cacheable, nil
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
