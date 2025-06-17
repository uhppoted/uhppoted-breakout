package main

type command interface {
	exec() error
}
