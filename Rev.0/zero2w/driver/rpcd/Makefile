UDS = unix::/tmp/uhppoted/breakout/rpc.sock
.DEFAULT_GOAL := build

update:
	go get -u ./...
	go mod tidy

format:
	go fmt ./...

build: format
	go build -o ./bin/ ./...

test: build
	go test ./...

run-tcp: build
	./bin/rpcd --bind "tcp:::1234"

run-unix: build
	./bin/rpcd --bind "$(UDS)"

run: run-unix
