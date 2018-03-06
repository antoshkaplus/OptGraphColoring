
file(GLOB SOURCE_FILES "./src/*.cpp")
file(GLOB HEADER_FILES "./include/*.hpp")

include_directories(
    ./include 
    /usr/local/include)


find_library(ARMA_LIBRARY armadillo)
if (NOT ARMA_LIBRARY) 
    message(FATAL_ERROR "armadillo library not found")    
endif()
find_library(ANT_LIBRARY ant)
if (NOT ANT_LIBRARY)
    message(FATAL_ERROR "ant library not found")
endif()


add_executable(euclidian_tsp ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(euclidian_tsp ${ARMA_LIBRARY} ${ANT_LIBRARY})

