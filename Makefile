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

TOOLCHAIN_VER=01.01.01
TOOLCHAIN_ROOT=/opt/toolchain/toolchain-arm-linux
TOOLCHAIN_DIR=$(TOOLCHAIN_ROOT)-$(TOOLCHAIN_VER)
IRVINE_SW_INSTALL_DIR=$(HOME)/.irvine-01-sw

all: build installDir
	$(MAKE) -C build

all-arm: build-arm installDir
	$(MAKE) -C build-arm

installDir:
	mkdir -p $(IRVINE_SW_INSTALL_DIR)
	cp -r scripts $(IRVINE_SW_INSTALL_DIR)
	cp -r auth $(IRVINE_SW_INSTALL_DIR)

initialize: $(TOOLCHAIN_DIR)

build: initialize
	if [ ! -e build ]; then mkdir build; fi;
	(cd build; cmake ../)

test:
	(cd build; make test CTEST_OUTPUT_ON_FAILURE=TRUE)

build-arm: initialize
	if [ ! -e build-arm ]; then mkdir build-arm; fi;
	(cd build-arm; cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-linux.cmake ../)

install:
	$(MAKE) -C build install DESTDIR=$(DESTDIR)

install-arm:
	$(MAKE) -C build-arm install DESTDIR=$(DESTDIR)

clean: 
	rm -rf build build-arm

distclean: clean clean-auth

#
# Retrieve the toolchain distribution and install it for the buildroot
# to use.  Run it as sudo to store it in /opt/toolchain.
#
$(TOOLCHAIN_DIR):  $(KEYINFO_FILE)
	sudo scripts/toolchainSetup.sh -v $(TOOLCHAIN_VER)

KEYINFO_FILE=$(HOME)/.irvine-01.keyInfo
AUTH_FILE=$(HOME)/.polysat_fsw.auth

genKeys: 
	$(MAKE) $(KEYINFO_FILE)

authfile: clean-auth
	$(MAKE) $(AUTH_FILE)

$(KEYINFO_FILE):
	@scripts/opensslKeyTool.sh -f $(KEYINFO_FILE) -g $$USER-irvine-01-sw

$(AUTH_FILE):
	@cp auth/access.enc $(AUTH_FILE).enc
	scripts/opensslKeyTool.sh -f $(KEYINFO_FILE) -d $(AUTH_FILE).enc

clean-auth:
	rm -f $(AUTH_FILE)

clean-keyinfo:
	rm -f $(KEYINFO_FILE)

clean-toolchain:
	-sudo rm $(TOOLCHAIN_DIR) $(TOOLCHAIN_ROOT)
