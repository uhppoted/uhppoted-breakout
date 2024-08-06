.DEFAULT_GOAL := build
.PHONY: format

format:
	go fmt ./...

build: format
	go build -o ./bin/ ./...

run: build
	./bin/emulator --usb /dev/tty.usbmodem14201

get-controller:
	../../../uhppote-cli/bin/uhppote-cli get-device 405419896
