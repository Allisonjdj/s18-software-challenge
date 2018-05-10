cmake_minimum_required(VERSION 3.1.0)
project(arduino-challenge CXX)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE WLIB_FILES ../pkg/wlib/*.h ../pkg/wlib/*.cpp)
add_library(wlib ${WLIB_FILES})
target_compile_definitions(wlib PUBLIC MAX_ALLOCATORS=10u NUM_BLOCKS=400u)
target_include_directories(wlib PUBLIC ../pkg/wlib)

file(GLOB_RECURSE MOCK_COSA mock-cosa/*.hh mock-cosa/*.cpp)
add_library(cosa ${MOCK_COSA})
target_include_directories(cosa PUBLIC mock-cosa)

file(GLOB_RECURSE PROJ_FILES ../src/*.h ../src/*.cpp)
add_executable(arduino_challenge ${PROJ_FILES} loop.h main.cpp)
target_link_libraries(arduino_challenge cosa wlib)
target_include_directories(arduino_challenge PUBLIC ../pkg/wlib mock-cosa)
