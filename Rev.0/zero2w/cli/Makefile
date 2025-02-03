ADDRESS ?= '127.0.0.1:60000'
TLS ?= 'localhost:60443'

format: 
	yapf -ri *.py

build: format
	python3 -m compileall .

debug:
	python3 main.py get-controller

tls:
	python3 main.py --destination $(TLS) --protocol tls get-controller

get-controller:
	python3 main.py get-controller
	python3 main.py --destination $(ADDRESS) --protocol udp get-controller
	python3 main.py --destination $(ADDRESS) --protocol tcp get-controller
	python3 main.py --destination $(TLS)     --protocol tls get-controller

set-IPv4:
	python3 main.py set-IPv4
	python3 main.py --destination $(ADDRESS) --protocol udp set-IPv4
	python3 main.py --destination $(ADDRESS) --protocol tcp set-IPv4

get-listener:
	python3 main.py get-listener
	python3 main.py --destination $(ADDRESS) --protocol udp get-listener
	python3 main.py --destination $(ADDRESS) --protocol tcp get-listener

set-listener:
	python3 main.py set-listener
	python3 main.py --destination $(ADDRESS) --protocol udp set-listener
	python3 main.py --destination $(ADDRESS) --protocol tcp set-listener

get-time:
	python3 main.py get-time
	python3 main.py --destination $(ADDRESS) --protocol udp get-time
	python3 main.py --destination $(ADDRESS) --protocol tcp get-time

set-time:
	python3 main.py set-time
	python3 main.py --destination $(ADDRESS) --protocol udp set-time
	python3 main.py --destination $(ADDRESS) --protocol tcp set-time

get-status:
	python3 main.py get-status
	python3 main.py --destination $(ADDRESS) --protocol udp get-status
	python3 main.py --destination $(ADDRESS) --protocol tcp get-status

get-door:
	python3 main.py get-door
	python3 main.py --destination $(ADDRESS) --protocol udp get-door
	python3 main.py --destination $(ADDRESS) --protocol tcp get-door

set-door:
	python3 main.py set-door
	python3 main.py --destination $(ADDRESS) --protocol udp set-door
	python3 main.py --destination $(ADDRESS) --protocol tcp set-door

set-door-passcodes:
	python3 main.py set-door-passcodes
	python3 main.py --destination $(ADDRESS) --protocol udp set-door-passcodes
	python3 main.py --destination $(ADDRESS) --protocol tcp set-door-passcodes

open-door:
	python3 main.py open-door
	python3 main.py --destination $(ADDRESS) --protocol udp open-door
	python3 main.py --destination $(ADDRESS) --protocol tcp open-door


get-cards:
	python3 main.py get-cards
	python3 main.py --destination $(ADDRESS) --protocol udp get-cards
	python3 main.py --destination $(ADDRESS) --protocol tcp get-cards

get-card:
	python3 main.py get-card
	python3 main.py --destination $(ADDRESS) --protocol udp get-card
	python3 main.py --destination $(ADDRESS) --protocol tcp get-card

get-card-by-index:
	python3 main.py get-card-by-index
	python3 main.py --destination $(ADDRESS) --protocol udp get-card-by-index
	python3 main.py --destination $(ADDRESS) --protocol tcp get-card-by-index

put-card:
	python3 main.py put-card
	python3 main.py --destination $(ADDRESS) --protocol udp put-card
	python3 main.py --destination $(ADDRESS) --protocol tcp put-card

delete-card:
	python3 main.py delete-card
	python3 main.py --destination $(ADDRESS) --protocol udp delete-card
	python3 main.py --destination $(ADDRESS) --protocol tcp delete-card

delete-all-cards:
	python3 main.py delete-all-cards
	python3 main.py --destination $(ADDRESS) --protocol udp delete-all-cards
	python3 main.py --destination $(ADDRESS) --protocol tcp delete-all-cards

record-special-events:
	python3 main.py record-special-events
	python3 main.py --destination $(ADDRESS) --protocol udp record-special-events
	python3 main.py --destination $(ADDRESS) --protocol tcp record-special-events

get-event-index:
	python3 main.py get-event-index
	python3 main.py --destination $(ADDRESS) --protocol udp get-event-index
	python3 main.py --destination $(ADDRESS) --protocol tcp get-event-index




