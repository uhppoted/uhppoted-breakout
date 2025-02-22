TINYGO = ~/Development/tools/tinygo/bin/tinygo
UDS = unix::/tmp/uhppoted/breakout/rpc.sock
.DEFAULT_GOAL := build

update:
	go get -u ./...
	go mod tidy

format:
	go fmt ./...

build: format
	go build -o ./bin/ ./...

tinygo-build:
	$(TINYGO) version	

test: build
	go test ./...

debug: build
	go test -v -timeout 500ms ./... -run TestDecodeGetRequest

run-tcp: build
	./bin/rpcd --bind "tcp:::1234"

run-unix: build
	./bin/rpcd --bind "$(UDS)"

run: run-unix

zero2w-build: format
	mkdir -p dist/arm7
	env GOOS=linux GOARCH=arm GOARM=7 GOWORK=off go build -trimpath -o dist/arm7 ./...

zero2w-run: zero2w-build
	scp dist/arm7/rpcd breakout:/opt/breakout/rpcd/rpcd
	ssh breakout "cd /opt/breakout/rpcd && ./rpcd --bind tcp:::1234"
