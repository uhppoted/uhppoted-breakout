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

run: build
	./bin/emulator --config .config.rpc

stub: build
	./bin/emulator --config .config.stub
