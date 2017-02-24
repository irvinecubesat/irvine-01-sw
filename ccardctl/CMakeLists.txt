add_library(ccardCodec CCardMsgCodec.cpp CCardMsgCodec.h CCardI2CPortState.cpp CCardI2CPortState.h ccardDefs.h)
add_subdirectory(test)
target_include_directories(ccardCodec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
