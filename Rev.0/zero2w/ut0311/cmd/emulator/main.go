package main

import (
	"crypto/sha256"
	"flag"
	"os"
	"os/signal"
	"slices"
	"syscall"
	"time"

	"emulator/UT0311"
	"emulator/config"
	"emulator/log"
)

const VERSION = "v0.0"

var options = struct {
	config string
}{
	config: ".config",
}

func main() {
	infof("uhppoted-breakout::emulator %v", VERSION)

	flag.StringVar(&options.config, "config", options.config, ".config file")
	flag.Parse()

	if cfg, hash, err := config.Load(options.config); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else if ut0311, err := UT0311.NewUT0311(&cfg); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else {
		go func() {
			ut0311.Run()
		}()

		go func() {
			watch(ut0311, options.config, hash)
		}()

		interrupt := make(chan os.Signal, 1)

		signal.Notify(interrupt, syscall.SIGINT, syscall.SIGTERM)

		<-interrupt

		ut0311.Stop()
	}
}

func watch(ut0311 *UT0311.UT0311, filepath string, hash []byte) {
	signature := hash

	f := func() {
		if bytes, err := os.ReadFile(filepath); err == nil {
			sha224 := sha256.Sum224(bytes)

			if !slices.Equal(signature, sha224[:]) {
				warnf("reloading config from %v", filepath)

				if cfg, _, err := config.Load(filepath); err != nil {
					warnf("%v", err)
				} else {
					ut0311.SetConfig(&cfg)
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
