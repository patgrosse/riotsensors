.PHONY: prepare

prepare:
	./prebuild.sh
	mkdir -p build
	cd build && cmake ..

build: prepare
	cd build && make