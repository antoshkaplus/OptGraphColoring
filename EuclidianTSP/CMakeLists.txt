
cmake_minimum_required(VERSION 3.1)
project(EuclidianTSP)

include(../Common/cmake/CMakeLists.txt)
include_directories("../Common/include")

file(GLOB SOURCE_FILES "./src/*.cpp")
file(GLOB HEADER_FILES "./include/*.hpp")

include_directories(
    ./include 
    /usr/local/include)
link_directories(
    /usr/local/lib
)


find_library(ARMA_LIBRARY armadillo)
if (NOT ARMA_LIBRARY) 
    message(FATAL_ERROR "armadillo library not found")    
endif()
find_library(ANT_LIBRARY ant)
if (NOT ANT_LIBRARY)
    message(FATAL_ERROR "ant library not found")
endif()

set_output_dirs()

add_executable(EuclidianTSP ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(EuclidianTSP ${ARMA_LIBRARY} ${ANT_LIBRARY})

file(GLOB_RECURSE BUILD_FILES "./benchmarks/*.cpp")
add_executable(TSP_BM ${BUILD_FILES})
target_link_libraries(TSP_BM benchmark pthread)

file(GLOB_RECURSE BUILD_FILES "./test/*.cpp")
add_executable(TSP_TEST ${BUILD_FILES})
target_link_libraries(TSP_TEST gtest pthread)

# analysis

add_executable(tsp_sa_analysis app/sa_analysis.cpp)
target_link_libraries(tsp_sa_analysis ant tbb)

add_executable(lk_analysis app/lk_analysis.cpp)
target_link_libraries(lk_analysis ant)

# methods

add_executable(tsp_simplex_ins app/methods.cpp)
target_link_libraries(tsp_simplex_ins ant)
target_compile_definitions(tsp_simplex_ins PUBLIC -DSIMPLEX_INS)

add_executable(tsp_shortest_edge app/methods.cpp)
target_link_libraries(tsp_shortest_edge ant)
target_compile_definitions(tsp_shortest_edge PUBLIC -DSHORTEST_EDGE)

add_executable(tsp_random_ins app/methods.cpp)
target_link_libraries(tsp_random_ins ant)
target_compile_definitions(tsp_random_ins PUBLIC -DRANDOM_INS)

add_executable(tsp_nearest_neighbor app/methods.cpp)
target_link_libraries(tsp_nearest_neighbor ant)
target_compile_definitions(tsp_nearest_neighbor PUBLIC -DNEAREST_NEIGHBOR)

add_executable(tsp_nearest_ins app/methods.cpp)
target_link_libraries(tsp_nearest_ins ant)
target_compile_definitions(tsp_nearest_ins PUBLIC -DNEAREST_INS)

add_executable(tsp_farthest_ins app/methods.cpp)
target_link_libraries(tsp_farthest_ins ant)
target_compile_definitions(tsp_farthest_ins PUBLIC -DFARTHEST_INS)

add_executable(tsp_ant_colony app/methods.cpp)
target_link_libraries(tsp_ant_colony ant)
target_compile_definitions(tsp_ant_colony PUBLIC -DANT_COLONY)

add_executable(tsp_sa app/methods.cpp)
target_link_libraries(tsp_sa ant)
target_compile_definitions(tsp_sa PUBLIC -DSA)

add_executable(tsp_lin_kernighan app/methods.cpp)
target_link_libraries(tsp_lin_kernighan ant)
target_compile_definitions(tsp_lin_kernighan PUBLIC -DLIN_KERNIGHAN)

# other

file(GLOB_RECURSE OTHER_LKH_SOURCE_FILES "./other/LKH-2.0.9/SRC/*")
add_executable(other_lkh ${OTHER_LKH_SOURCE_FILES})
target_include_directories(other_lkh PRIVATE "./other/LKH-2.0.9/SRC/INCLUDE")
target_link_libraries(other_lkh m)