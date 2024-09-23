SDK ?=$(HOME)/Development/tools/pico/pico-sdk
PORT=/dev/tty.usbmodem14201

.PHONY: format
.PHONY: telnet

clean: 
	rm -rf build
	mkdir -p build
	export PICO_SDK_PATH=$(SDK) && \
	       cd build && \
	       cmake ..

format: 
	clang-format -i src/*.c
	clang-format -i src/crypt/hash/*.c             include/crypt/hash/*.h
	clang-format -i src/crypt/hmac/*.c             include/crypt/hmac/*.h
	clang-format -i src/encoding/ASN.1/*.c         include/encoding/ASN.1/*.h
	clang-format -i ../core/src/*.c                ../core/include/*.h
	clang-format -i ../lib/src/*.c                 ../lib/include/*.h
	clang-format -i ../lib/src/types/*.c           ../lib/include/types/*.h
	clang-format -i ../lib/src/encoding/ASN.1/*.c  ../lib/include/encoding/ASN.1/*.h
	clang-format -i ../lib/src/encoding/bisync/*.c ../lib/include/encoding/bisync/*.h
	clang-format -i ../lib/src/encoding/ssmp/*.c   ../lib/include/encoding/ssmp/*.h
	clang-format -i ../lib/src/crypt/hash/*.c      ../lib/include/crypt/hash/*.h
	clang-format -i ../lib/src/crypt/hmac/*.c      ../lib/include/crypt/hmac/*.h
	clang-format -i ../lib/src/crypt/otp/*.c       ../lib/include/crypt/otp/*.h

build: format
	export PICO_SDK_PATH=$(SDK) && cd build && make clean && make

run: build
	picotool load -v build/breakout.uf2 -f

tty:
	 minicom -b 115200 -o -D $(PORT)
