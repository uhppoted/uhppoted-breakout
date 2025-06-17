TINYGO = ~/Development/tools/tinygo/bin/tinygo
TCP = tcp:::1234
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


# run-tcp: build
# 	./bin/rpcd --bind "$(TCP)" --device stub

# run-unix: build
# 	./bin/rpcd --bind "$(UDS)" --device stub

# run: run-unix

zero2w-build: format
	mkdir -p dist/arm7
	env GOOS=linux GOARCH=arm GOARM=7 GOWORK=off go build -trimpath -o dist/arm7 ./...

zero2w-run: zero2w-build
	scp dist/arm7/rpcd breakout:/opt/breakout/rpcd/rpcd
	ssh breakout "cd /opt/breakout/rpcd && ./rpcd --bind "$(TCP)" --device /dev/serial0"

zer02w: zerow2w-run


rpi3-build: format
	mkdir -p dist/arm6
	env GOOS=linux GOARCH=arm GOARM=6 GOWORK=off go build -trimpath -o dist/arm6 ./...

rpi3-run: rpi3-build
	scp dist/arm6/rpcd phil:/opt/uhppoted/uhppoted-breakout/rpcd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/rpcd && \
	            ./rpcd --bind "$(TCP)" \
	                   --dial tcp::192.168.1.100:4321 \
	                   --device /dev/serial/by-id/usb-uhppoted_breakout_E6625C05E7415B25-if02"

rpi3-daemonize: rpi3-build
	scp dist/arm6/rpcd phil:/opt/uhppoted/uhppoted-breakout/rpcd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/rpcd && \
	            sudo ./rpcd daemonize \
	                   --bind "$(TCP)" \
	                   --dial tcp::192.168.1.100:4321 \
	                   --device /dev/serial/by-id/usb-uhppoted_breakout_E6625C05E7415B25-if02"

rpi3-undaemonize: rpi3-build
	scp dist/arm6/rpcd phil:/opt/uhppoted/uhppoted-breakout/rpcd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/rpcd && \
	            ./rpcd undaemonize"

run: rpi3-run

daemonize: rpi3-daemonize

undaemonize: rpi3-undaemonize
