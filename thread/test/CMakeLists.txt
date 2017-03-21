file(GLOB SRCS "*.cpp")
add_executable(threadTest ${SRCS} )
#target_include_directories(threadTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}../)

target_link_libraries(threadTest irvThreadUtil gtest gtest_main)

add_test(ThreadTest threadTest)

IF (PROJECT_COMPILE)
# Include directories to search in
#target_include_directories(ccardctlTest PUBLIC /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
