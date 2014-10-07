cmake_minimum_required(VERSION 2.8.4)
project(GraphColoring)

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -O3")

file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB HEADER_FILES "src/*.h")

set(SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES})

add_executable(GraphColoring ${SOURCE_FILES})

target_include_directories(GraphColoring PUBLIC "/usr/local/include")

find_library(ANT_LIBRARY ant)
target_link_libraries(GraphColoring ${ANT_LIBRARY})

