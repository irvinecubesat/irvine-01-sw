file(GLOB SRCS "*.cpp")
add_executable(cmdCodecTest ${SRCS})

target_link_libraries(cmdCodecTest ccardCmdCodec gtest gtest_main)

add_test(CmdCodecTest cmdCodecTest)
