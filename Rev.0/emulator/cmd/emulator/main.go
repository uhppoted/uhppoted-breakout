package main

import (
	"crypto/sha256"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"slices"
	"syscall"
	"time"

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

	if cfg, hash, err := UT0311.Load(options.config); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else if options.USB == "" {
		errorf("invalid USB TTY device")
		os.Exit(1)
	} else {
		ut0311 := UT0311.UT0311{}

		go func() {
			ssmp.SSMP{
				USB: options.USB,
			}.Run()
		}()

		go func() {
			ut0311.Run(cfg)
		}()

		go func() {
			watch(&ut0311, options.config, hash)
		}()

		interrupt := make(chan os.Signal, 1)

		signal.Notify(interrupt, syscall.SIGINT, syscall.SIGTERM)

		<-interrupt
		fmt.Printf("  ... interrupted\n")
	}
}

func watch(ut0311 *UT0311.UT0311, filepath string, hash []byte) {
	signature := hash

	f := func() {
		if bytes, err := os.ReadFile(filepath); err == nil {
			sha224 := sha256.Sum224(bytes)

			if !slices.Equal(signature, sha224[:]) {
				warnf("reloading config from %v", filepath)

				if cfg, _, err := UT0311.Load(filepath); err != nil {
					warnf("%v", err)
				} else {
					ut0311.SetConfig(cfg)
				}

				signature = sha224[:]
			}
		}
	}

	tick := time.Tick(15 * time.Second)

	for {
		<-tick
		f()
	}
}

func infof(format string, args ...any) {
	log.Infof("emulator", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("emulator", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("emulator", format, args...)
}
