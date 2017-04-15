include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

add_library(gpioCpp Gpio.cpp Gpio.h)
target_include_directories(gpioCpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(gpioUtil gpioUtil.cpp)
target_link_libraries(gpioUtil gpioCpp )
INSTALL(TARGETS gpioUtil DESTINATION bin COMPONENT binaries)

IF (PROJECT_COMPILE)
add_subdirectory(test)
# Include directories to search in
ENDIF (PROJECT_COMPILE)
