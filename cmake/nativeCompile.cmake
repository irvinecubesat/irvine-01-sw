#
# Host (x86_64) libraries are located in the toolchain lib directory.
# Only add these libraries if we're not crosscompiling.
# The cmake/arm-linux.cmake file is used during crosscompiling and
# arm libs are under sysroot of the toolchain
#
IF (NOT CMAKE_CROSSCOMPILING)
  link_directories(/opt/toolchain/toolchain-arm-linux/lib /opt/toolchain/toolchain-arm-linux/lib64)
ENDIF (NOT CMAKE_CROSSCOMPILING)
