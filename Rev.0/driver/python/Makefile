.PHONY: format

format: 
	yapf -ri src
	yapf -ri tests

build: format
	python3 -m compileall .

test: build
	python3 -m unittest tests/ssmp/*.py 
