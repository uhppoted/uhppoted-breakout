package main

import (
	"fmt"
)

type daemonize struct {
}

func makeDaemonize() (daemonize, error) {
	return daemonize{}, fmt.Errorf("-- not supported --")
}

func (d daemonize) exec() error {
	return fmt.Errorf("-- not supported --")
}
