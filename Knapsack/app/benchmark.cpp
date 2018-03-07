
#include <benchmark/benchmark.h>

#include "support.h"
#include "knapsack.h"
#include "knapsack_opt.h"


void Knapsack_BM(benchmark::State& state) {
    auto path = "data/ks_" + std::to_string(state.range(0)) + "_0";
    std::ifstream in(path);
    auto pr = ReadProblem(in);

    for (auto _ : state) {
        Knapsack knapsack;
        knapsack.relaxation(pr.items, pr.capacity);
    }
}

void KnapsackOpt_BM(benchmark::State& state) {
    auto path = "data/ks_" + std::to_string(state.range(0)) + "_0";
    std::ifstream in(path);
    auto pr = ReadProblem(in);

    for (auto _ : state) {
        KnapsackOpt knapsack;
        knapsack.relaxation(pr.items, pr.capacity);
    }
}

void Usual_BM(benchmark::State& state) {
    auto path = "data/ks_" + std::to_string(state.range(0)) + "_0";
    std::ifstream in(path);
    auto pr = ReadProblem(in);

    for (auto _ : state) {
        usual(pr.items, pr.capacity);
    }
}

void UsualOpt_BM(benchmark::State& state) {
    auto path = "data/ks_" + std::to_string(state.range(0)) + "_0";
    std::ifstream in(path);
    auto pr = ReadProblem(in);

    for (auto _ : state) {
        usualOpt(pr.items, pr.capacity);
    }
}

BENCHMARK(Knapsack_BM)->Arg(30)->Arg(50)->Arg(100);
BENCHMARK(KnapsackOpt_BM)->Arg(30)->Arg(200)->Arg(10000);
BENCHMARK(Usual_BM)->Arg(30)->Arg(200);
BENCHMARK(UsualOpt_BM)->Arg(30)->Arg(200)->Arg(10000);


int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}
