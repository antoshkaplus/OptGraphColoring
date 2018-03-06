
find_library(ANT_LIBRARY ant)

include_directories(include)

add_executable(FacilityLocation_Solver src/main.cpp)
target_link_libraries(FacilityLocation_Solver glpk)


