.PHONY: build-all

build-all:
	cd Rev.0/zero2w/ut0311      && make build
	cd Rev.0/firmware/tiny-uart && make clean && make build
	cd Rev.0/firmware/test      && make clean && make build
