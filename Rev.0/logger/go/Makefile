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
	./bin/breakout-log --device $(PORT)
