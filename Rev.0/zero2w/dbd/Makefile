TCP = tcp:::12346
UDS = unix::/tmp/uhppoted/breakout/dbd.sock
SQLITE = sqlite:///var/uhppoted/breakout/db/sqlite3/ut0311.db

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

debug: build
	echo "** nothing to do"


run-tcp: build
	./bin/dbd --bind "$(TCP)" --DSN "sqlite3://../workdir/db/sqlite3/ut0311.db"

run-unix: build
	./bin/dbd --bind "$(UDS)" --DSN "sqlite3://../workdir/db/sqlite3/ut0311.db"


zero2w-build: format
	mkdir -p dist/arm7
	env GOOS=linux GOARCH=arm GOARM=7 GOWORK=off go build -trimpath -o dist/arm7 ./...

zero2w-run: zero2w-build
	scp dist/arm7/dbd breakout:/opt/breakout/dbd
	ssh breakout "cd /opt/breakout/dbd && ./dbd --bind "$(TCP)" --DSN "sqlite3:///var/uhppoted/breakout/db/sqlite3/ut0311.db"

zer02w: zerow2w-run


rpi3-build: format
	mkdir -p dist/arm6
	env GOOS=linux GOARCH=arm GOARM=6 GOWORK=off go build -trimpath -o dist/arm6 ./...

rpi3-run: rpi3-build
	scp dist/arm6/dbd phil:/opt/uhppoted/uhppoted-breakout/dbd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/dbd && ./dbd --bind $(TCP) --DSN 'sqlite:///var/uhppoted/breakout/db/sqlite3/ut0311.db'"

rpi3: rpi3-run

rpi3-daemonize: rpi3-build
	ssh -t phil "sudo systemctl stop uhppoted-breakout-dbd"
	scp dist/arm6/dbd phil:/opt/uhppoted/uhppoted-breakout/dbd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/dbd && sudo ./dbd daemonize --bind $(TCP) --DSN $(SQLITE)"

rpi3-undaemonize: rpi3-build
	ssh -t phil "sudo systemctl stop uhppoted-breakout-dbd"
	scp dist/arm6/dbd phil:/opt/uhppoted/uhppoted-breakout/dbd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/dbd && sudo ./dbd undaemonize"

run: rpi3-run

daemonize: rpi3-daemonize

undaemonize: rpi3-undaemonize

