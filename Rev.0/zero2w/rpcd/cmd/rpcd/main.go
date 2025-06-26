package main

import (
	"fmt"
	"os"

	"ssmp/log"
)

const VERSION = "v0.0.0"
const SERVICE = `uhppoted-breakout-rpcd`

const DEFAULT_DEVICE = ""
const DEFAULT_BIND = "tcp:::1234"
const DEFAULT_DIAL = "tcp:::4321"

func main() {
	fmt.Printf("%v  %v\n", SERVICE, VERSION)

	if cmd, err := parse(); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else if err := cmd.exec(); err != nil {
		errorf("%v", err)
		os.Exit(1)
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
