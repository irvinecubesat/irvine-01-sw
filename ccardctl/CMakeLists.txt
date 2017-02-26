include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

add_library(ccardCodec CCardMsgCodec.cpp CCardMsgCodec.h CCardI2CPortState.cpp CCardI2CPortState.h ccardDefs.h)
target_include_directories(ccardCodec PUBLIC ../../../staging/usr/include/i2c-tools/ ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(ccardctl CCardCtl.cpp CCardI2CX.cpp CCardI2CX.h)
target_link_libraries(ccardctl ccardCodec proc pthread)

add_executable(ccardcmd CCardCmd.cpp)
target_link_libraries(ccardcmd ccardCodec proc pthread)

INSTALL(TARGETS ccardctl DESTINATION bin COMPONENT binaries)
INSTALL(TARGETS ccardcmd DESTINATION bin COMPONENT binaries)
INSTALL(FILES ccardctl.cmd.cfg DESTINATION /etc/ COMPONENT config) 


IF (PROJECT_COMPILE)
add_subdirectory(test)
# Include directories to search in
target_include_directories(ccardctl PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)
target_include_directories(ccardcmd PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
