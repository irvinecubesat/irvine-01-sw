file(GLOB SRCS "*.cpp")
add_executable(beaconTest ${SRCS})

target_link_libraries(beaconTest beaconUtil gtest gtest_main)

add_test(BeaconTest beaconTest)
