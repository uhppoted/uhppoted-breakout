package main

import (
	"fmt"
	"os"

	"dbd/log"
)

const SERVICE = "uhppoted-breakout-dbd"
const VERSION = "v0.0.0"
const LOGTAG = "dbd"

const DEFAULT_BIND = "tcp:::12346"
const DEFAULT_DSN = ""

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
	log.Infof(LOGTAG, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(LOGTAG, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(LOGTAG, format, args...)
}
