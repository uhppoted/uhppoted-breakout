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

run: build
	./bin/emulator --config .config.rpc

stub: build
	./bin/emulator --config .config.stub

get-controller:
	../../../../uhppote-cli/bin/uhppote-cli get-device 405419896
