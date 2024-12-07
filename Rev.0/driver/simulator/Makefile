.PHONY: format

format: 
	yapf -ri src

build: format
	python3 -m compileall .

run: build
	python3 src/main.py
