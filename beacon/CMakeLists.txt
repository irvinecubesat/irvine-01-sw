include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

#
# The beacon executable is build using the beacon.c source file
#
add_executable(beacon beacon.c beacon.cmd.cfg)

#
# Install the beacon binary to the bin directory when the "install" target is
# called
#
INSTALL(TARGETS beacon DESTINATION bin COMPONENT binaries)
INSTALL(FILES beacon.cmd.cfg DESTINATION /etc/ COMPONENT config) 

#
# Libraries that beacon depends on
#
target_link_libraries(beacon proc)

IF (PROJECT_COMPILE)
# Include directories to search in
target_include_directories(beacon PUBLIC /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
