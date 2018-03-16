
#include <benchmark/benchmark.h>

#include "GC.hpp"


template<class GC_Solver>
void BM_GC(benchmark::State& state) {
    GC_Solver solver;

    ifstream in(Format(std::string("data/gc_%d_%d"), state.range(0), state.range(1)));
    auto gr = ReadProblem(in);
    ColoredGraph c_gr;
    for (auto _ : state) {
        c_gr = solver.solve(gr);
    }
}


BENCHMARK_TEMPLATE(BM_GC, GC_Naive)->Args({100, 1})->Args({100, 5})->Args({100, 7});
BENCHMARK_TEMPLATE(BM_GC, GC_Naive_2)->Args({100, 1})->Args({100, 5})->Args({100, 7});


int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}