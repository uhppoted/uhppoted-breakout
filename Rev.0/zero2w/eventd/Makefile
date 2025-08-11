TCP = tcp:::12346
UDS = unix::/tmp/uhppoted/breakout/events.sock
SQLITE = sqlite:///var/uhppoted/breakout/db/sqlite3/ut0311.db?_journal_mode=WAL

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
	./bin/eventd --bind "$(TCP)" --DSN "sqlite3://../workdir/db/sqlite3/ut0311.db"

run-unix: build
	./bin/eventd --bind "$(UDS)" --DSN "sqlite3://../workdir/db/sqlite3/ut0311.db"


zero2w-build: format
	mkdir -p dist/arm7
	env GOOS=linux GOARCH=arm GOARM=7 GOWORK=off go build -trimpath -o dist/arm7 ./...

zero2w-run: zero2w-build
	scp dist/arm7/eventd breakout:/opt/breakout/eventd
	ssh breakout "cd /opt/breakout/eventd && ./eventd --bind "$(TCP)" --DSN "sqlite3:///var/uhppoted/breakout/db/sqlite3/ut0311.db"

zer02w: zerow2w-run


rpi3-build: format
	mkdir -p dist/arm6
	env GOOS=linux GOARCH=arm GOARM=6 GOWORK=off go build -trimpath -o dist/arm6 ./...

rpi3-run: rpi3-build
	scp dist/arm6/eventd phil:/opt/uhppoted/uhppoted-breakout/eventd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/eventd && ./eventd --bind $(TCP) --DSN 'sqlite:///var/uhppoted/breakout/db/sqlite3/ut0311.db?_journal_mode=WAL'"

rpi3: rpi3-run

rpi3-daemonize: rpi3-build
	ssh -t phil "sudo systemctl stop uhppoted-breakout-eventd"
	scp dist/arm6/eventd phil:/opt/uhppoted/uhppoted-breakout/eventd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/eventd && sudo ./eventd daemonize --bind $(TCP) --DSN $(SQLITE)"

rpi3-undaemonize: rpi3-build
	ssh -t phil "sudo systemctl stop uhppoted-breakout-eventd"
	scp dist/arm6/eventd phil:/opt/uhppoted/uhppoted-breakout/eventd
	ssh -t phil "cd /opt/uhppoted/uhppoted-breakout/eventd && sudo ./eventd undaemonize"

run: rpi3-run

daemonize: rpi3-daemonize

undaemonize: rpi3-undaemonize

