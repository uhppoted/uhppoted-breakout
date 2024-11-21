PORT=/dev/tty.usbmodem14201

.DEFAULT_GOAL := build
.PHONY: format

update:
	go get -u ./...
	go mod tidy

format:
	go fmt ./...

build: format
	go build -o ./bin/ ./...

test: build
	go test ./...

run: build
	./bin/emulator --usb /dev/tty.usbmodem14201

get-controller:
	../../../uhppote-cli/bin/uhppote-cli get-device 405419896

tty:
	 minicom -b 115200 -o -D $(PORT)

encode: build
	./bin/ssmp 

