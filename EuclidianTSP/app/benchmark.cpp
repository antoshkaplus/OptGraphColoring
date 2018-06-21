
#include <benchmark/benchmark.h>

#include "tsp.hpp"

class DistanceCompute {

    const std::vector<Point>& ps;

public:
    DistanceCompute(const std::vector<Point>& ps) : ps(ps) {}

    double dist(Index i_1, Index i_2) const {
        return ps[i_1].Distance(ps[i_2]);
    }
};


class DistanceCache {

    arma::Mat<double> mat;

public:
    DistanceCache(const std::vector<Point>& ps) : mat(ps.size(), ps.size()) {
        grid::Region(0, 0, ps.size(), ps.size()).ForEach([&](auto pos) {
            mat(pos.row, pos.col) = ps[pos.row].Distance(ps[pos.col]);
        });
    }

    double dist(Index i_1, Index i_2) const {
        return mat(i_1, i_2);
    }
};


template<class Distance>
void BM_TSP_Distance(benchmark::State& state) {

    // maybe pass some templated argument that will give distance back,
    // need just one looping method

    std::vector<Edge> order;
    // will have to fill it up
    std::vector<Point> ps;


    Distance dist(ps);
    for (auto& e : order) {
        benchmark::DoNotOptimize(dist(e[0], e[1]));
    }
}


BENCHMARK_TEMPLATE(BM_GC, GC_Naive)->Args({100, 1})->Args({100, 5})->Args({100, 7});
BENCHMARK_TEMPLATE(BM_GC, GC_Naive_2)->Args({100, 1})->Args({100, 5})->Args({100, 7});


BENCHMARK_TEMPLATE(BM_GC_SA, true)->Args({500,1})->Args({500,3})->Args({500,5})->Args({500,7})->Args({500,9})->
Args({1000,1})->Args({1000,3})->Args({1000,5})->Args({1000,7})->Args({1000,9});
BENCHMARK_TEMPLATE(BM_GC_SA, false)->Args({500,1})->Args({500,3})->Args({500,5})->Args({500,7})->Args({500,9})->
Args({1000,1})->Args({1000,3})->Args({1000,5})->Args({1000,7})->Args({1000,9});


int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}