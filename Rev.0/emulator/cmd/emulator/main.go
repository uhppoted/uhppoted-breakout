package main

import (
	"flag"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/UT0311"
	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/log"
	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/ssmp"
)

const VERSION = "v0.0"

var options = struct {
	config string
	USB    string
}{
	config: ".config",
	USB:    "",
}

func main() {
	infof("uhppoted-breakout::emulator %v", VERSION)

	flag.StringVar(&options.config, "config", options.config, ".config file")
	flag.StringVar(&options.USB, "usb", options.USB, "USB TTY device")
	flag.Parse()

	if cfg, err := UT0311.Load(options.config); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else if options.USB == "" {
		errorf("invalid USB TTY device")
		os.Exit(1)
	} else {
		go func() {
			ssmp.SSMP{
				USB: options.USB,
			}.Run()
		}()

		go func() {
			UT0311.UT0311{}.Run(cfg)
		}()

		interrupt := make(chan os.Signal, 1)

		signal.Notify(interrupt, syscall.SIGINT, syscall.SIGTERM)

		<-interrupt
		fmt.Printf("  ... interrupted\n")
	}
}

func infof(format string, args ...any) {
	log.Infof("emulator", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("emulator", format, args...)
}
