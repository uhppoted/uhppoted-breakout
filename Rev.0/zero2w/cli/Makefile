ADDRESS ?= '127.0.0.1:60000'
TLS ?= 'localhost:60443'
CONTROLLER ?= 405419896
DOOR ?= 1

format: 
	yapf -ri *.py

build: format
	python3 -m compileall .

debug:
	python3 main.py set-event-index --controller $(CONTROLLER) --index 23

get-controller:
	python3 main.py get-controller
#	python3 main.py --destination $(ADDRESS) --protocol udp get-controller
#	python3 main.py --destination $(ADDRESS) --protocol tcp get-controller
#	python3 main.py --destination $(TLS)     --protocol tls get-controller

set-IPv4:
	python3 main.py set-IPv4

get-listener:
	python3 main.py get-listener

set-listener:
	python3 main.py set-listener

get-time:
	python3 main.py get-time

set-time:
	python3 main.py set-time

get-status:
	python3 main.py get-status

get-door:
	python3 main.py get-door --controller $(CONTROLLER) --door $(DOOR)

set-door:
	python3 main.py set-door --controller $(CONTROLLER) --door $(DOOR) --mode controlled --delay 5

set-door-passcodes:
	python3 main.py set-door-passcodes

open-door:
	python3 main.py open-door

get-cards:
	python3 main.py get-cards

get-card:
	python3 main.py get-card

get-card-by-index:
	python3 main.py get-card-by-index

put-card:
	python3 main.py put-card

delete-card:
	python3 main.py delete-card

delete-all-cards:
	python3 main.py delete-all-cards

record-special-events:
	python3 main.py record-special-events --controller $(CONTROLLER) --enabled true

get-event-index:
	python3 main.py get-event-index --controller $(CONTROLLER)

set-event-index:
	python3 main.py set-event-index --controller $(CONTROLLER) --index 37

get-event:
	python3 main.py get-event --controller $(CONTROLLER) --event 13

listen:
	python3 main.py listen
