ADDRESS ?= '127.0.0.1:60000'

format: 
	yapf -ri *.py

build: format
	python3 -m compileall .

get-controller:
	python3 main.py get-controller
	python3 main.py --destination $(ADDRESS) --udp get-controller
	python3 main.py --destination $(ADDRESS) --tcp get-controller

set-IPv4:
	python3 main.py set-IPv4
	python3 main.py --destination $(ADDRESS) --udp set-IPv4
	python3 main.py --destination $(ADDRESS) --tcp set-IPv4

get-listener:
	python3 main.py get-listener
	python3 main.py --destination $(ADDRESS) --udp get-listener
	python3 main.py --destination $(ADDRESS) --tcp get-listener
