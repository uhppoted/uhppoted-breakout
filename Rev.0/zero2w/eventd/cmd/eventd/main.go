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
	"eventd/eventd"
	"eventd/log"
)

const VERSION = "v0.0.0"
const LOGTAG = "eventd"

var options = struct {
	bind string
	DSN  string
}{
	bind: "tcp:::12346",
}

func main() {
	fmt.Printf("uhppoted-breakout::eventd %v\n", VERSION)

	flag.StringVar(&options.bind, "bind", options.bind, "bind address (in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flag.StringVar(&options.DSN, "DSN", options.DSN, "DB DSN e.g. sqlite3:///var/uhppoted/breakout/db/sqlite3/ut0311.db")
	flag.Parse()

	if d, err := eventd.NewEventD(options.bind); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else if d == nil {
		errorf("invalid EventsD (%v)", d)
		os.Exit(1)
	} else if err := db.Init(options.DSN); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else {
		var wg sync.WaitGroup

		wg.Add(1)
		go func() {
			d.Run()
			wg.Done()
		}()

		terminated := make(chan struct{})
		interrupt := make(chan os.Signal, 1)

		signal.Notify(interrupt, syscall.SIGINT, syscall.SIGTERM)

		<-interrupt

		d.Stop()

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
	log.Infof(LOGTAG, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(LOGTAG, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(LOGTAG, format, args...)
}
