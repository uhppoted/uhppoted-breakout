ADDRESS ?= '127.0.0.1:60000'
TLS ?= 'localhost:60443'

format: 
	yapf -ri *.py

build: format
	python3 -m compileall .

debug:
	python3 main.py get-controller
	python3 main.py get-time

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
	python3 main.py get-door

set-door:
	python3 main.py set-door

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
	python3 main.py record-special-events

get-event-index:
	python3 main.py get-event-index

set-event-index:
	python3 main.py set-event-index
