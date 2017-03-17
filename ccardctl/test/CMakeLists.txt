file(GLOB SRCS "*.cpp")
add_executable(ccardctlTest ${SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../InitialDeployOp.cpp)
target_include_directories(ccardctlTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}../)

target_link_libraries(ccardctlTest ccardCodec gtest gtest_main proc)

add_test(CCardCtlTest ccardctlTest)

IF (PROJECT_COMPILE)
# Include directories to search in
target_include_directories(ccardctlTest PUBLIC /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
