SDK ?= $(HOME)/Development/tools/pico/pico-sdk
RPI ?= phil
PORT ?= /dev/tty.usbmodem14201
BUILD ?= Debug

RX8900SA_VDET ?= disabled
RX8900SA_SWOFF ?= switch
RX8900SA_BKSMP ?= 2ms

CONTROLLER ?= 405419896
VERSION ?= 0x0001
RELEASED ?= "$(shell date '+%Y-%m-%d')"
TRACE ?= 10

.PHONY: format

clean: 
	@echo "--- $(BUILD) build ---" | tr '[:lower:]' '[:upper:]'
	rm -rf build
	mkdir -p build
	export PICO_SDK_PATH=$(SDK) && \
	       cd build && \
	       cmake -DCMAKE_BUILD_TYPE=$(BUILD) \
	             -DMODE="CLI" \
	             -DSSMP="USB" \
	             -DWATCHDOG="enabled" \
	             -DI2C0_CLOCK="100kHz" -DI2C1_CLOCK="400kHz" \
	             -DRX8900SA_VDET="$(RX8900SA_VDET)" -DRX8900SA_SWOFF="$(RX8900SA_SWOFF)" -DRX8900SA_BKSMP="$(RX8900SA_BKSMP)" \
	             -DCONTROLLER=$(CONTROLLER) \
	             -DVERSION=$(VERSION)       \
	             -DRELEASED=$(RELEASED)     \
	             -DTRACE=$(TRACE)           \
	             ..

format: 
	clang-format -i src/*.c
	clang-format -i ../core/src/*.c                ../core/include/*.h
	clang-format -i ../core/src/*.c                ../core/include/*.h
	clang-format -i ../lib/src/*.c                 ../lib/include/*.h
	clang-format -i ../lib/src/types/*.c           ../lib/include/types/*.h
	clang-format -i ../lib/src/encoding/ASN.1/*.c  ../lib/include/encoding/ASN.1/*.h
	clang-format -i ../lib/src/encoding/bisync/*.c ../lib/include/encoding/bisync/*.h
	clang-format -i ../lib/src/encoding/ssmp/*.c   ../lib/include/encoding/ssmp/*.h
	clang-format -i ../lib/src/crypt/hash/*.c      ../lib/include/crypt/hash/*.h
	clang-format -i ../lib/src/crypt/otp/*.c       ../lib/include/crypt/otp/*.h

build: format
	export PICO_SDK_PATH=$(SDK) && cd build && make

release: BUILD=Release
release: clean build

run-local: build
	picotool load -v build/breakout.uf2 -f

remote-run: build
	scp build/breakout.uf2 $(RPI):/tmp
	ssh $(RPI) "picotool load -v /tmp/breakout.uf2 -F"
	ssh $(RPI) "picotool reboot"

run: remote-run

reboot-local: 
	picotool reboot --vid 0x2e8a --pid 0x0003 -f

reboot-remote: 
	ssh $(RPI) "picotool reboot --vid 0x2e8a --pid 0x0003 -f -u"

reboot: reboot-remote

tty:
	 minicom -b 115200 -o -D $(PORT)
