SDK ?= $(HOME)/Development/tools/pico/pico-sdk
RPI ?= phil
PORT ?= /dev/tty.usbmodem14201

RX8900SA_VDET ?= disabled
RX8900SA_SWOFF ?= switch
RX8900SA_BKSMP ?= 2ms

CONTROLLER ?= 405419896
VERSION ?= 0x0001
RELEASED := "$(shell date '+%Y-%m-%d %H:%M')"

.PHONY: format

clean: 
	rm -rf build
	mkdir -p build
	export PICO_SDK_PATH=$(SDK) && \
	       cd build && \
	       cmake -DMODE="log" \
	             -DWATCHDOG="enabled" \
	             -DI2C0_CLOCK="100kHz" -DI2C1_CLOCK="400kHz" \
	             -DRX8900SA_VDET="$(RX8900SA_VDET)" -DRX8900SA_SWOFF="$(RX8900SA_SWOFF)" -DRX8900SA_BKSMP="$(RX8900SA_BKSMP)" \
	             -DCONTROLLER=$(CONTROLLER) \
	             -DVERSION=$(VERSION)       \
	             -DRELEASED=$(RELEASED)     \
	             ..

format: 
	clang-format -i src/*.c

build: format
	export PICO_SDK_PATH=$(SDK) && cd build && make clean && make

local-run: build
	picotool load -v build/breakout.uf2 -f

remote-run: build
	scp build/mvb.uf2 $(RPI):/tmp
	ssh $(RPI) "picotool load -v /tmp/mvb.uf2 -F"
	ssh $(RPI) "picotool reboot"

run: remote-run

reboot: 
	picotool reboot -f

tty:
	 minicom -b 115200 -o -D $(PORT)
