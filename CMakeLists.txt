cmake_minimum_required(VERSION 2.8.4)
project(GraphColoring)

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -O3")

# will initialize in subdirectory
set(SOURCE_FILES ${SOURCE_FILES})

add_subdirectory("${CMAKE_SOURCE_DIR}/src")

add_executable(GraphColoring ${SOURCE_FILES})


