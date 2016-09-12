# assume the tool chain is one level above us
# resolve the relative path to toolchain into an absolute path
get_filename_component(CROSSTOOL_PATH ${PROJECT_SOURCE_DIR}/../toolchain-arm-linux-01.00.02 ABSOLUTE)

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_COMPILER ${CROSSTOOL_PATH}/bin/arm-linux-gcc)
SET(CMAKE_CXX_COMPILER ${CROSSTOOL_PATH}/bin/arm-linux-g++)
SET(CMAKE_FIND_ROOT_PATH ${CROSSTOOL_PATH})
# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

