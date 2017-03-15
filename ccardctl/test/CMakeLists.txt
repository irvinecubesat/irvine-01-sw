file(GLOB SRCS "*.cpp")
add_executable(ccardctlTest ${SRCS})

target_link_libraries(ccardctlTest ccardCodec gtest gtest_main)

add_test(CCardCtlTest ccardctlTest)
