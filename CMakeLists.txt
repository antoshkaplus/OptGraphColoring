# cmake -DCMAKE_BUILD_TYPE=Debug
# cmake -DCMAKE_BUILD_TYPE=Release

cmake_minimum_required(VERSION 3.1)
project(GraphColoring)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wno-sign-compare")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native")

set(BinDir "../bin")
set(LibDir "../lib")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinDir} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LibDir} )
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BinDir} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LibDir} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

include_directories("/usr/local/include")
include_directories("/usr/include")

add_subdirectory(EuclidianTSP)
add_subdirectory(FacilityLocation)
add_subdirectory(GraphColoring)
add_subdirectory(Knapsack)
add_subdirectory(VehicleRouting)

#
#find_library(ANT_LIBRARY ant)
#
#
#add_library(GraphColoring STATIC ${SOURCE_FILES} ${HEADER_FILES})
#target_link_libraries(GraphColoring ${ANT_LIBRARY})
#
#add_executable(Solver ${SOLVER_FILES})
#target_link_libraries(Solver
#        ${ANT_LIBRARY}
#        GraphColoring)
#
#target_include_directories(GraphColoring PUBLIC "/usr/local/include")
#
#find_library(ANT_LIBRARY ant)
#target_link_libraries(GraphColoring ${ANT_LIBRARY})

