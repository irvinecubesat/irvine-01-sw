file(GLOB SRCS "*.cpp")
add_executable(gpioTest ${SRCS})

target_link_libraries(gpioTest gpioCpp gtest_main)

add_test(GPIOTest gpioTest)
