
#include <benchmark/benchmark.h>

#include "GC.hpp"
#include "GC_SA.hpp"


template<class GC_Solver>
void BM_GC(benchmark::State& state) {
    GC_Solver solver;

    ifstream in(Format("data/gc_%d_%d", state.range(0), state.range(1)));
    auto gr = ReadProblem(in);
    ColoredGraph c_gr;
    for (auto _ : state) {
        c_gr = solver.solve(gr);
    }
}

BENCHMARK_TEMPLATE(BM_GC, GC_Naive)->Args({100, 1})->Args({100, 5})->Args({100, 7});
BENCHMARK_TEMPLATE(BM_GC, GC_Naive_2)->Args({100, 1})->Args({100, 5})->Args({100, 7});


template<bool use_binary_search>
void BM_GC_SA(benchmark::State& state) {
    const auto max_iterations = 10000;
    GC_SA<CoolingSchedule_1, use_binary_search> solver(CoolingSchedule_1(state.range(0), max_iterations), max_iterations);
    ifstream in(Format("data/gc_%d_%d", state.range(0), state.range(1)));
    auto gr = ReadProblem(in);
    ColoredGraph c_gr;
    for (auto _ : state) {
        c_gr = solver.solve(gr);
    }
}

BENCHMARK_TEMPLATE(BM_GC_SA, true)->Args({500,1})->Args({500,3})->Args({500,5})->Args({500,7})->Args({500,9})->
        Args({1000,1})->Args({1000,3})->Args({1000,5})->Args({1000,7})->Args({1000,9});
BENCHMARK_TEMPLATE(BM_GC_SA, false)->Args({500,1})->Args({500,3})->Args({500,5})->Args({500,7})->Args({500,9})->
        Args({1000,1})->Args({1000,3})->Args({1000,5})->Args({1000,7})->Args({1000,9});


int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}