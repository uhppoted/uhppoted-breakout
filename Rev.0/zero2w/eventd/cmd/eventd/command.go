package main

import (
	"os"
)

type command interface {
	exec() error
}

func parse() (command, error) {
	if len(os.Args) > 1 {
		cmd := os.Args[1]

		switch cmd {
		case "daemonize":
			return makeDaemonize()

		case "undaemonize":
			return makeUndaemonize()
		}
	}

	return makeRun()
}
