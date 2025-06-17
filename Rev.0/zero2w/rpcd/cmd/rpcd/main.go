package main

import (
	"fmt"
	"os"

	"ssmp/log"
)

const VERSION = "v0.0.0"

var options = struct {
	device string
	bind   string
	dial   string
}{
	device: "",
	bind:   "tcp:::1234",
	dial:   "tcp:::4321",
}

func main() {
	fmt.Printf("SSMP RPC DRIVER %v\n", VERSION)

	if cmd, err := parse(); err != nil {
		errorf("%v", err)
		os.Exit(1)
	} else if err := cmd.exec(); err != nil {
		errorf("%v", err)
		os.Exit(1)
	}
}

func parse() (command, error) {
	// var cmd commands.Command = nil
	// var err error = nil

	// if len(os.Args) > 1 {
	// 	for _, c := range cli {
	// 		if c.CLI() == flag.Arg(0) {
	// 			cmd = c
	// 		}
	// 	}
	// }

	return makeRun()
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
