#
# Makefile to kick off cmake and build under the "build" directory
#
# to build x86 binaries in build directory:
#
# make all
#
# to build arm binaries under the build-arm directory:
#
# make all-arm
#
#

TOOLCHAIN_VER=01.00.02
TOOLCHAIN_ROOT=/opt/toolchain/toolchain-arm-linux

all: build
	$(MAKE) -C build

all-arm: build-arm
	$(MAKE) -C build-arm

build:
	-mkdir build
	(cd build; cmake ../)

build-arm: $(TOOLCHAIN_ROOT)
	-mkdir build-arm
	(cd build-arm; cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-linux.cmake ../)

install:
	$(MAKE) -C build install DESTDIR=$(DESTDIR)

install-arm:
	$(MAKE) -C build-arm install DESTDIR=$(DESTDIR)

clean:
	rm -rf build build-arm

#
# Retrieve the toolchain distribution and install it for the buildroot
# to use.  Run it as sudo to store it in /opt/toolchain.
#
$(TOOLCHAIN_ROOT):
	sudo scripts/toolchainSetup.sh -v $(TOOLCHAIN_VER)

clean-toolchain:
	-sudo rm $(TOOLCHAIN_ROOT)
