#
# Host (x86_64) libraries are located in the toolchain lib directory.
# Only add these libraries if we're not crosscompiling.
# The cmake/arm-linux.cmake file is used during crosscompiling and
# arm libs are under sysroot of the toolchain
#
IF (NOT CMAKE_CROSSCOMPILING)
  link_directories(/opt/toolchain/toolchain-arm-linux/lib)
ENDIF (NOT CMAKE_CROSSCOMPILING)

#
# The beacon executable is build using the beacon.c source file
#
add_executable(beacon beacon.c)

#
# Install the beacon binary to the bin directory when the "install" target is
# called
#
INSTALL(FILES beacon DESTINATION bin)

#
# Libraries that beacon depends on
#
target_link_libraries(beacon proc)

# Include directories to search in
target_include_directories(beacon PUBLIC /opt/toolchain/toolchain-arm-linux/include)

