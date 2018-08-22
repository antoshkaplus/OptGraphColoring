
#include "benchmarks/util.hpp"

#include "tour/tsp_two_level_tree_tour.hpp"
#include "tour/tsp_vector_tour.hpp"

template<class Tour>
void BM_TSP_Tour(benchmark::State& state) {
    auto city_count = state.range(0);

    Tour tour(city_count);
    for (auto _ : state) {
        for (auto p : cityPairs(city_count, city_count)) {
            tour.Reverse(p[0], p[1]);
        }
    }
}

BENCHMARK_TEMPLATE(BM_TSP_Tour, VectorTour)->Arg(1000)->Arg(5000)->Arg(10'000)->Arg(50'000)->Arg(100'000); //->Arg(1000'000);
BENCHMARK_TEMPLATE(BM_TSP_Tour, TwoLevelTreeTour)->Arg(1000)->Arg(5000)->Arg(10'000)->Arg(50'000)->Arg(100'000); //->Arg(1000'000);

//BENCHMARK_TEMPLATE(BM_TSP_Tour, VectorTour)->Arg(1000)->Arg(5000)->Arg(10'000);
//BENCHMARK_TEMPLATE(BM_TSP_Tour, TwoLevelTreeTour)->Arg(1000)->Arg(5000)->Arg(10'000);