.PHONY: build-all

build-all:
	cd Rev.0/emulator && make build
	cd Rev.0/firmware/tiny && make clean && make build
	cd Rev.0/firmware/pico && make clean && make build