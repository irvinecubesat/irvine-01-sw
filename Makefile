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
KEYTOOL=scripts/opensslKeyTool.sh

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

# Run unit tests with minimal messages unless we get an error
check-v:
	(cd build; ctest -V)

# Run unit tests with verbose messages
check:
	(cd build; make test CTEST_OUTPUT_ON_FAILURE=1)

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

KEYINFO_FILE=$(shell if [ -e $(HOME)/.irvine-01.keyInfo ]; then echo $(HOME)/.irvine-01.keyInfo;  else echo $(HOME)/.irvinecubesat.keyInfo; fi)
AUTH_FILE=$(HOME)/.polysat_fsw.auth
IRVINE-02-ACCESS=auth/irv-02-access.enc

# short host name
HOST_NAME=$(shell hostname -s)

printKeyInfoFile:
	echo $(KEYINFO_FILE)

genKeys: 
	$(MAKE) $(KEYINFO_FILE)
	scripts/cubeSatNetSetupRequest.sh

authfile: clean-auth
	$(MAKE) $(AUTH_FILE)

$(KEYINFO_FILE):
	@$(KEYTOOL) -f $(KEYINFO_FILE) -g $$USER-$(HOST_NAME)-irvinecubesat

$(AUTH_FILE):
	@cp auth/access.enc $(AUTH_FILE).enc
	@$(KEYTOOL) -f $(KEYINFO_FILE) -d $(AUTH_FILE).enc -o $(AUTH_FILE)

irv-02-access:
	@$(KEYTOOL) -f $(KEYINFO_FILE) -d $(IRVINE-02-ACCESS)

clean-auth:
	rm -f $(AUTH_FILE)

clean-keyinfo:
	rm -f $(KEYINFO_FILE)

clean-toolchain:
	-sudo rm $(TOOLCHAIN_DIR) $(TOOLCHAIN_ROOT)
