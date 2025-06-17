package main

import (
	"fmt"
)

type undaemonize struct {
}

func makeUndaemonize() (undaemonize, error) {
	return undaemonize{}, fmt.Errorf("-- not supported --")
}

func (u undaemonize) exec() error {
	return fmt.Errorf("-- not supported --")
}
