SDK ?= $(HOME)/Development/tools/pico/pico-sdk
PORT ?= /dev/tty.usbmodem14201
BUILD ?= Debug
VERSION ?= 0x0001

.PHONY: format

clean: 
	rm -rf build
	mkdir -p build
	export PICO_SDK_PATH=$(SDK) && cd build && cmake -DVERSION=$(VERSION) -DCMAKE_BUILD_TYPE=Debug ..

format: 
	clang-format -i src/*.c

build: format
	export PICO_SDK_PATH=$(SDK) && cd build && make

run: build
	picotool load -v build/stub.uf2 -f

reboot: 
	ssh $(RPI) "picotool reboot"

tty:
	minicom -b 115200 -o -D $(PORT)
