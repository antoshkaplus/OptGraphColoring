include_directories(include)

set_output_dirs()

add_executable(Knapsack_Solver app/main.cpp)

add_executable(Knapsack_BM app/benchmark.cpp)
target_link_libraries(Knapsack_BM benchmark ${COIN_LIBS})

add_executable(Knapsack_Test app/test.cpp)
target_link_libraries(Knapsack_Test gtest pthread ${COIN_LIBS})

add_executable(Knapsack_LP app/lp.cpp)
target_link_libraries(Knapsack_LP ${COIN_LIBS})
