.DEFAULT_GOAL := build
.PHONY: format

format:
	go fmt ./...

build: format
	go build -o ./bin/ ./...

run: build
	./bin/emulator --usb /dev/tty.usbmodem14201
