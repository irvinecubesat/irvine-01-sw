include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

add_library(ccardCodec CCardMsgCodec.cpp CCardMsgCodec.h CCardI2CPortState.cpp CCardI2CPortState.h ccardDefs.h)
add_subdirectory(test)
target_include_directories(ccardCodec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(ccardctl CCardCtl.cpp CCardI2CX.cpp CCardI2CX.h)
target_link_libraries(ccardctl ccardCodec proc polydrivers m dl rt pthread)

INSTALL(TARGETS ccardctl DESTINATION bin COMPONENT binaries)
#INSTALL(TARGETS ccardcmd DESTINATION bin COMPONENT binaries)
INSTALL(FILES ccardctl.cmd.cfg DESTINATION /etc/ COMPONENT config) 


IF (PROJECT_COMPILE)
# Include directories to search in
target_include_directories(ccardctl PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
