.PHONY: build

build:
	./prebuild.sh
	mkdir -p build
	cmake ..
	make