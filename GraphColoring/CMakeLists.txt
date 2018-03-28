
file(GLOB SOLVER_FILES "app/solver.cpp")

find_library(ANT_LIBRARY ant)

include_directories(include)

add_executable(GraphColoring_Solver ${SOLVER_FILES})
target_link_libraries(GraphColoring_Solver
        ${ANT_LIBRARY})

add_executable(GC_BM app/benchmark.cpp)
target_link_libraries(GC_BM benchmark pthread)

add_executable(GC_LP app/lp.cpp)
target_link_libraries(GC_LP ant ${COIN_LIBS})

add_executable(GC_LP_compare app/lp_compare.cpp)
target_link_libraries(GC_LP_compare ant ${COIN_LIBS})


# methods

add_executable(gc_naive app/methods.cpp)
target_link_libraries(gc_naive ant)
target_compile_definitions(gc_naive PUBLIC -DNAIVE)

add_executable(gc_backtrack app/methods.cpp)
target_link_libraries(gc_backtrack ant)
target_compile_definitions(gc_backtrack PUBLIC -DBACKTRACK)

add_executable(gc_hill_climbing app/methods.cpp)
target_link_libraries(gc_hill_climbing ant)
target_compile_definitions(gc_hill_climbing PUBLIC -DHILL_CLIMBING)

add_executable(gc_ga app/methods.cpp)
target_link_libraries(gc_ga ant)
target_compile_definitions(gc_ga PUBLIC -DGA)

add_executable(gc_rec_large_first app/methods.cpp)
target_link_libraries(gc_rec_large_first ant)
target_compile_definitions(gc_rec_large_first PUBLIC -DREC_LARGE_FIRST)

add_executable(gc_ts app/methods.cpp)
target_link_libraries(gc_ts ant)
target_compile_definitions(gc_ts PUBLIC -DTS)

add_executable(gc_tabucol app/methods.cpp)
target_link_libraries(gc_tabucol ant)
target_compile_definitions(gc_tabucol PUBLIC -DTABUCOL)

add_executable(gc_kempe_chain app/methods.cpp)
target_link_libraries(gc_kempe_chain ant)
target_compile_definitions(gc_kempe_chain PUBLIC -DKEMPE_CHAIN)

add_executable(gc_lp app/methods.cpp)
target_link_libraries(gc_lp ant ${COIN_LIBS})
target_compile_definitions(gc_lp PUBLIC -DLP)