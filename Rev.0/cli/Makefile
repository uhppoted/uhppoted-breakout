ADDRESS ?= '127.0.0.1:60000'
TLS ?= 'localhost:60443'

format: 
	yapf -ri *.py

build: format
	python3 -m compileall .

debug:
	python3 main.py --destination $(TLS) --protocol tls get-controller

get-controller:
	python3 main.py get-controller
	python3 main.py --destination $(ADDRESS) --protocol udp get-controller
	python3 main.py --destination $(ADDRESS) --protocol tcp get-controller

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
	python3 main.py --destination $(ADDRESS) --protocol udp get-listener
	python3 main.py --destination $(ADDRESS) --protocol tcp get-listener

get-time:
	python3 main.py get-time
	python3 main.py --destination $(ADDRESS) --protocol udp get-time
	python3 main.py --destination $(ADDRESS) --protocol tcp get-time

