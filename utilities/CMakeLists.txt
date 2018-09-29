include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

add_executable(thruster ThrusterThrottle.cpp)
target_link_libraries(thruster m)

add_executable(mag MagCtl.cpp)


IF (PROJECT_COMPILE)
# Include directories to search in
target_include_directories(ccardctl PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)
target_include_directories(ccardcmd PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
