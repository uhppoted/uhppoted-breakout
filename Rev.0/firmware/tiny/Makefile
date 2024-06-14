SDK ?=$(HOME)/Development/tools/pico/pico-sdk
PORT=/dev/tty.usbmodem14201
SRC=src/*.c ../core/src/*.c ../lib/src/*.c
INCLUDE=include/*.h ../core/include/*.h ../lib/include/*.h

.PHONY: format

clean: 
	rm -rf build
	mkdir -p build
	export PICO_SDK_PATH=$(SDK) && \
	       cd build && \
	       cmake ..

format: 
	clang-format -i src/*.c         -i include/*.h
	clang-format -i ../core/src/*.c -i ../core/include/*.h
	clang-format -i ../lib/src/*.c  -i ../lib/include/*.h

build: format
	export PICO_SDK_PATH=$(SDK) && cd build && make clean && make

run: build
	picotool load -v build/breakout.uf2 -f

tty:
	 minicom -b 115200 -o -D $(PORT)
