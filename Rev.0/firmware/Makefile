SDK ?=$(HOME)/Development/tools/pico/pico-sdk
PORT=/dev/tty.usbmodem14201

clean:
	rm -rf build
	mkdir -p build
	export PICO_SDK_PATH=$(SDK) && \
	       cd build && \
	       cmake ..

format: src/*.c
	clang-format -i src/*.c

build: format
	export PICO_SDK_PATH=$(SDK) && cd build && make clean && make

install: build
	cp build/breakout.uf2 /Volumes/RPI-RP2

tty:
	 minicom -b 115200 -o -D $(PORT)
