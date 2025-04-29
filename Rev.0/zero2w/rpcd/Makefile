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
	go test -v -timeout 500ms ./... -run TestEncodeSetUint8Request

run-tcp: build
	./bin/rpcd --bind "tcp:::1234" --device stub --cacheable .cacheable.json

run-unix: build
	./bin/rpcd --bind "$(UDS)" --device stub --cacheable .cacheable.json

run: run-unix

zero2w-build: format
	mkdir -p dist/arm7
	env GOOS=linux GOARCH=arm GOARM=7 GOWORK=off go build -trimpath -o dist/arm7 ./...

zero2w-run: zero2w-build
	scp dist/arm7/rpcd breakout:/opt/breakout/rpcd/rpcd
	ssh breakout "cd /opt/breakout/rpcd && ./rpcd --bind tcp:::1234 --device /dev/serial0" --cacheable .cacheable.json


zer02w: zerow2w-run

rpi3-build: format
	mkdir -p dist/arm6
	env GOOS=linux GOARCH=arm GOARM=6 GOWORK=off go build -trimpath -o dist/arm6 ./...

rpi3-run: rpi3-build
	scp dist/arm6/rpcd phil:/opt/uhppoted/uhppoted-breakout/rpcd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/rpcd && \
	            ./rpcd --bind tcp:::1234 \
	                   --dial tcp::192.168.1.100:4321 \
	                   --device /dev/serial/by-id/usb-uhppoted_breakout_E6625C05E7415B25-if02" \
	                   --cacheable .cacheable.json

rpi3: rpi3-run


