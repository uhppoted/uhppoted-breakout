SDK ?=$(HOME)/Development/tools/pico/pico-sdk
PORT=/dev/tty.usbmodem14201

RX8900SA_VDET ?= disabled
RX8900SA_SWOFF ?= switch
RX8900SA_BKSMP ?= 2ms

CONTROLLER ?= 405419896
VERSION ?= 0x0001

.PHONY: format

clean: 
	rm -rf build
	mkdir -p build
	export PICO_SDK_PATH=$(SDK) && \
	       cd build && \
	       cmake -DI2C0_CLOCK="100kHz" -DI2C1_CLOCK="400kHz" \
	             -DRX8900SA_VDET="$(RX8900SA_VDET)" -DRX8900SA_SWOFF="$(RX8900SA_SWOFF)" -DRX8900SA_BKSMP="$(RX8900SA_BKSMP)" \
	             -DCONTROLLER=$(CONTROLLER) \
	             -DVERSION=$(VERSION) \
	             ..

format: 
	clang-format -i src/*.c
	clang-format -i ../core/src/*.c                ../core/include/*.h
	clang-format -i ../lib/src/*.c                 ../lib/include/*.h
	clang-format -i ../lib/src/types/*.c           ../lib/include/types/*.h
	clang-format -i ../lib/src/encoding/ASN.1/*.c  ../lib/include/encoding/ASN.1/*.h
	clang-format -i ../lib/src/encoding/bisync/*.c ../lib/include/encoding/bisync/*.h
	clang-format -i ../lib/src/encoding/ssmp/*.c   ../lib/include/encoding/ssmp/*.h
	clang-format -i ../lib/src/crypt/hash/*.c      ../lib/include/crypt/hash/*.h
	clang-format -i ../lib/src/crypt/otp/*.c       ../lib/include/crypt/otp/*.h

build: format
	export PICO_SDK_PATH=$(SDK) && cd build && make clean && make

run: build
	picotool load -v build/breakout.uf2 -f

tty:
	 minicom -b 115200 -o -D $(PORT)
