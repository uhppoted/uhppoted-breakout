.DEFAULT_GOAL := build

update:
#	go get -u ./...
	go get -u github.com/uhppoted/uhppote-core@main
	go mod tidy

format:
	go fmt ./...

build: format
	go build -o ./bin/ ./...

test: build
	go test ./...

run-tcp: build
	./bin/emulator --config .config.localhost

run-unix: build
	./bin/emulator --config .config.unix

run-breakout: build
	./bin/emulator --config .config.zero2w

run: run-breakout

stub: build
	./bin/emulator --config .config.stub
