include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

#
# The beacon executable is build using the beacon.c source file
#
add_executable(cmdd cmdd.cpp)
add_executable(cmdExec cmdExec.cpp)

#
# Install the beacon binary to the bin directory when the "install" target is
# called
#
INSTALL(TARGETS cmdd DESTINATION bin COMPONENT binaries)
INSTALL(TARGETS cmdExec DESTINATION bin COMPONENT binaries)
INSTALL(FILES cmdd.cmd.cfg DESTINATION /etc/ COMPONENT config) 

#
# Libraries that beacon depends on
#
target_link_libraries(cmdd proc pthread)
target_link_libraries(cmdExec proc pthread)

IF (PROJECT_COMPILE)
# Include directories to search in
target_include_directories(cmdd PUBLIC /opt/toolchain/toolchain-arm-linux/include)
target_include_directories(cmdExec PUBLIC /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
