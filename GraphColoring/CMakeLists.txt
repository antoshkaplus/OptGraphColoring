
file(GLOB SOLVER_FILES "app/solver.cpp")

find_library(ANT_LIBRARY ant)

include_directories(include)

add_executable(GraphColoring_Solver ${SOLVER_FILES})
target_link_libraries(GraphColoring_Solver
        ${ANT_LIBRARY})

add_executable(GC_BM app/benchmark.cpp)
target_link_libraries(GC_BM benchmark pthread)


# methods

add_executable(gc_naive app/methods.cpp)
target_link_libraries(gc_naive ant)
target_compile_definitions(gc_naive PUBLIC -DNAIVE)

add_executable(gc_backtrack app/methods.cpp)
target_link_libraries(gc_backtrack ant)
target_compile_definitions(gc_backtrack PUBLIC -DBACKTRACK)