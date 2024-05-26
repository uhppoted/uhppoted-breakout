SDK ?=$(HOME)/Development/tools/pico/pico-sdk
PORT=/dev/tty.usbmodem14201

clean:
	rm -rf build
	mkdir -p build
	export PICO_SDK_PATH=$(SDK) && \
	       cd build && \
	       cmake ..

format: src/*.c
	clang-format -i src/*.c         include/*.h
	clang-format -i ../core/src/*.c ../core/include/*.h
	clang-format -i ../lib/src/*.c  ../lib/include/*.h

build: format
	export PICO_SDK_PATH=$(SDK) && cd build && make clean && make

install: build
	# picotool load build/breakout.uf2
	# picotool reboot
	cp build/breakout.uf2 /Volumes/RPI-RP2

run: build
	openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -c "program build/breakout.elf verify reset exit"

tty:
	 minicom -b 115200 -o -D $(PORT)
