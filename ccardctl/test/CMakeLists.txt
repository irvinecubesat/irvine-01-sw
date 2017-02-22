file(GLOB SRCS "*.cpp")
add_executable(msgCodecTest ${SRCS})

target_link_libraries(msgCodecTest ccardCodec gtest gtest_main)

add_test(MsgCodecTest msgCodecTest)
