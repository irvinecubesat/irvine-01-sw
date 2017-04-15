include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

#
# The beacon executable is build using the beacon.c source file
#
add_library(beaconUtil BeaconStatus.cpp BeaconStatus.h)
target_include_directories(beaconUtil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(beacon beacon.c beacon.cmd.cfg)
add_executable(beaconDecoder beaconDecoder.cpp)
target_link_libraries(beaconDecoder beaconUtil)

#
# Install the beacon binary to the bin directory when the "install" target is
# called
#
INSTALL(TARGETS beacon DESTINATION bin COMPONENT binaries)
INSTALL(FILES beacon.cmd.cfg DESTINATION /etc/ COMPONENT config) 

#
# Libraries that beacon depends on
#
target_link_libraries(beacon proc pthread)

IF (PROJECT_COMPILE)
  # temporarily omit Beacon tests until comms implements them
  #  add_subdirectory(test)
  # Include directories to search in
  target_include_directories(beacon PUBLIC /opt/toolchain/toolchain-arm-linux/include)
  target_include_directories(beaconDecoder PUBLIC /opt/toolchain/toolchain-arm-linux/include)
  target_include_directories(beaconUtil PUBLIC /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
