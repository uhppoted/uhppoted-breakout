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

debug: build
	go test ./... -run TestOIDEqual

run-tcp: build
	./bin/ut0311 --config .config.localhost

run-unix: build
	./bin/ut0311 --config .config.unix

run-breakout: build
	./bin/ut0311 --config .config.zero2w

run: run-unix

stub: build
	./bin/ut0311 --config .config.stub

rpi3: build
	./bin/ut0311 --config .config.rpi3
