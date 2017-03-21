include(${CMAKE_SOURCE_DIR}/cmake/nativeCompile.cmake)

add_library(irvThreadUtil Thread.cpp Thread.h Mutex.h MutexLock.h)
target_include_directories(irvThreadUtil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

IF (PROJECT_COMPILE)
  add_subdirectory(test)
ENDIF(PROJECT_COMPILE)
