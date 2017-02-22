include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

#
# The beacon executable is build using the beacon.c source file
#
add_executable(sensorManager sensorManager.cpp)

#
# Install the beacon binary to the bin directory when the "install" target is
# called
#
INSTALL(TARGETS sensorManager DESTINATION bin COMPONENT binaries) 

#
# Libraries that beacon depends on
#
target_link_libraries(sensorManager proc pthread)

IF (PROJECT_COMPILE)
# Include directories to search in
target_include_directories(sensorManager PUBLIC /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
