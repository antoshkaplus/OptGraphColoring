include_directories(include)

add_executable(Knapsack_Solver app/main.cpp)

add_executable(Knapsack_BM app/benchmark.cpp)
target_link_libraries(Knapsack_BM benchmark)

add_executable(Knapsack_Test app/test.cpp)
target_link_libraries(Knapsack_Test gtest pthread)
