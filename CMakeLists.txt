# cmake -DCMAKE_BUILD_TYPE=Debug
# cmake -DCMAKE_BUILD_TYPE=Release

cmake_minimum_required(VERSION 3.1)
project(GraphColoring)

set(CMAKE_CXX_FLAGS "-std=c++2a -Wno-sign-compare -Wall -Werror")
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Clang|AppleClang)")
    string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-private-field -Wno-missing-braces")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -march=native -DNDEBUG")

macro(set_output_dirs)
    set(BinDir ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    set(LibDir ${CMAKE_CURRENT_SOURCE_DIR}/lib)

    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinDir} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LibDir} )
    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BinDir} )
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LibDir} )
    endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endmacro()

include_directories("/usr/local/include")
include_directories("/usr/include")

list(APPEND COIN_LIBS CoinUtils Cbc Clp OsiClp Cgl)

add_subdirectory(EuclidianTSP)
add_subdirectory(FacilityLocation)
add_subdirectory(GraphColoring)
add_subdirectory(Knapsack)
add_subdirectory(VehicleRouting)
