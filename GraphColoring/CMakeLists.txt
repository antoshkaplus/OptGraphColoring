
file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.hpp")
file(GLOB SOLVER_FILES "app/solver.cpp")

find_library(ANT_LIBRARY ant)

include_directories(include)

add_library(GraphColoring STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(GraphColoring
        ${ANT_LIBRARY})

add_executable(GraphColoring_Solver ${SOLVER_FILES})
target_link_libraries(GraphColoring_Solver
    ${ANT_LIBRARY}
    GraphColoring)


