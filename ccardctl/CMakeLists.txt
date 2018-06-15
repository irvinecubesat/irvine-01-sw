include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

add_library(ccardCodec CCardMsgCodec.cpp CCardMsgCodec.h CCardI2CPortState.cpp CCardI2CPortState.h ccardDefs.h)
target_include_directories(ccardCodec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ccardctl CCardCtl.cpp CCardI2CX.cpp CCardI2CX.h DsaController.h InitialDsaDeployOp.cpp InitialDsaDeployOp.h DsaOp.cpp DsaOp.h DsaOpContext.h)
target_include_directories(ccardCodec PUBLIC ../../../staging/usr/include/i2c-tools/ )
target_link_libraries(ccardctl ccardCodec gpioCpp proc irvThreadUtil pthread)

add_executable(ccardcmd CCardCmd.cpp)
target_link_libraries(ccardcmd ccardCodec proc pthread)

INSTALL(TARGETS ccardctl DESTINATION bin COMPONENT binaries)
INSTALL(TARGETS ccardcmd DESTINATION bin COMPONENT binaries)
INSTALL(FILES ccardctl.cmd.cfg DESTINATION /etc/ COMPONENT config) 
INSTALL(FILES ccardTelemetry.cfg DESTINATION /usr/local/etc/dl_config COMPONENT config)
INSTALL(FILES powerMonTelemetry.cfg DESTINATION /usr/local/etc/dl_config COMPONENT config)

target_include_directories(ccardctl PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)
target_include_directories(ccardcmd PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)

IF (PROJECT_COMPILE)
add_subdirectory(test)
# Include directories to search in
#target_include_directories(ccardctl PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)
#target_include_directories(ccardcmd PUBLIC ${CMAKE_BINARY_DIR}/i2c-tools-3.1.0/usr/local/include /opt/toolchain/toolchain-arm-linux/include)
ENDIF (PROJECT_COMPILE)
