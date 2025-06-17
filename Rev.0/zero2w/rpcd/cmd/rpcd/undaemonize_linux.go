package main

import (
	"flag"
	"fmt"
	"os"
)

type undaemonize struct {
}

func makeUndaemonize() (undaemonize, error) {
	u := undaemonize{}

	fs := flag.NewFlagSet("undaemonize", flag.ExitOnError)
	fs.Parse(os.Args[2:])

	return u, nil
}

func (u undaemonize) exec() error {
	return fmt.Errorf("*** NOT IMPLEMENTED ***")
}
