#
# Makefile to kick off cmake and build under the "build" directory
#
#
all: build
	$(MAKE) -C build

all-arm: build-arm
	$(MAKE) -C build-arm

build:
	(mkdir build; cd build; cmake ../)

build-arm:
	(mkdir build-arm; cd build-arm; cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-linux.cmake ../)

install:
	$(MAKE) -C build install DESTDIR=$(DESTDIR)

install-arm:
	$(MAKE) -C build-arm install DESTDIR=$(DESTDIR)

clean:
	rm -rf build build-arm

