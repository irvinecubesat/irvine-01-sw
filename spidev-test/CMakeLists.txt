include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

add_executable(spidev-test spidev-test.c)

INSTALL(TARGETS spidev-test DESTINATION bin COMPONENT binaries)
