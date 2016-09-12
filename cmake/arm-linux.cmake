#
# Need to run toolchainSetup.sh to set up the following toolchain root
# 
SET(TOOLCHAIN_ROOT /opt/toolchain/toolchain-arm-linux)

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_COMPILER ${TOOLCHAIN_ROOT}/bin/arm-linux-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_ROOT}/bin/arm-linux-g++)
SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_ROOT})
# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

