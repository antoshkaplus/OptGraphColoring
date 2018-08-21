
#include <benchmark/benchmark.h>

#include "benchmarks/util.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"


const std::vector<Count> kBounds = {16, 64, 124};


static void CityCount_Bound_Args(benchmark::internal::Benchmark* b) {
    for (auto city_count : kCityCounts) {
        for (auto bound : kBounds) {
            b->Args({city_count, bound});
        }
        b->Args({city_count, static_cast<Count>(sqrt(city_count))});
    }
}

static void BM_TSP_TwoLevelTree(benchmark::State& state) {
    auto city_count = state.range(0);
    auto bound = state.range(1);

    auto pairs = cityPairs(city_count, city_count);
    TwoLevelTreeTour tour(city_count, bound);

    for (auto _ : state) {
        for (auto& p : pairs) {
            tour.Reverse(p[0], p[1]);
        }
    }
}

BENCHMARK(BM_TSP_TwoLevelTree)->Apply(CityCount_Bound_Args);


