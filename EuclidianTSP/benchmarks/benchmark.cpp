
#include <benchmark/benchmark.h>

#include <ant/grid/grid.hpp>

#include "tsp.hpp"
#include "sa/sa.hpp"
#include "tour/tsp_tour.hpp"
#include "tour/tsp_vector_tour.hpp"
#include "tour/tsp_tour_reverse.hpp"

template<class Distance>
void BM_TSP_Distance(benchmark::State& state) {

    auto N = state.range(0);
    std::vector<Edge> order(N);
    default_random_engine rng;
    uniform_int_distribution<Index> distr(0, N-1);
    generate(order.begin(), order.end(), [&]() {
        return Edge(distr(rng), distr(rng));
    });

    // don't need to fill
    std::vector<Point> ps(N);

    Distance dist(ps);
    for (auto _ : state) {
        for (auto& e : order) {
            benchmark::DoNotOptimize(dist(e[0], e[1]));
        }
    }
}


BENCHMARK_TEMPLATE(BM_TSP_Distance, DistanceCompute)->Arg(100)->Arg(500)->Arg(700)->Arg(1000)->Arg(2000);
BENCHMARK_TEMPLATE(BM_TSP_Distance, DistanceCache)->Arg(100)->Arg(500)->Arg(700)->Arg(1000)->Arg(2000);



class BM_TSP_VectorTourReverse_Fixture : public benchmark::Fixture {
protected:
    VectorTour tour;
    // to reverse
    vector<std::pair<Index, Index>> cityPairs;

public:
    void SetUp(const benchmark::State& state) override {
        default_random_engine rng;

        const Count city_count = state.range(0);

        vector<Index> cities(city_count);
        iota(cities.begin(), cities.end(), 0);
        shuffle(cities.begin(), cities.end(), rng);

        tour = VectorTour(cities);

        cityPairs.resize(city_count);
        uniform_int_distribution distr(0, city_count-1);

        for (auto& i : cityPairs) {
            i.first = distr(rng);
            i.second = distr(rng);
        }
    }
};

BENCHMARK_DEFINE_F(BM_TSP_VectorTourReverse_Fixture, Updater_1)(benchmark::State& st) {
    Updater_1_Factory factory;
    for (auto _ : st) {
        for (auto p : cityPairs) {
            tour.Reverse<Updater_1_Factory>(p.first, p.second, factory);
        }
    }
}

BENCHMARK_DEFINE_F(BM_TSP_VectorTourReverse_Fixture, Updater_2)(benchmark::State& st) {
    Updater_2_Factory factory(st.range(0));
    for (auto _ : st) {
        for (auto p : cityPairs) {
            tour.Reverse<Updater_2_Factory>(p.first, p.second, factory);
        }
    }
}

BENCHMARK_DEFINE_F(BM_TSP_VectorTourReverse_Fixture, UpdaterCombine)(benchmark::State& st) {
    UpdaterCombineFactory factory(st.range(0), st.range(1) / 100.);
    for (auto _ : st) {
        for (auto p : cityPairs) {
            tour.Reverse<UpdaterCombineFactory>(p.first, p.second, factory);
        }
    }
}

BENCHMARK_REGISTER_F(BM_TSP_VectorTourReverse_Fixture, Updater_1)->Arg(100)->Arg(1000)->Arg(2000)->Arg(5000)->Arg(10000)->Arg(50000);
BENCHMARK_REGISTER_F(BM_TSP_VectorTourReverse_Fixture, Updater_2)->Arg(100)->Arg(1000)->Arg(2000)->Arg(5000)->Arg(10000)->Arg(50000);

static void UpdaterCombineArgs(benchmark::internal::Benchmark* b) {
    for (auto city_count : {2000, 5000, 10000, 50000}) {
        for (auto threshold : {60, 70, 80, 90}) {
            b->Args({city_count, threshold});
        }
    }
}

BENCHMARK_REGISTER_F(BM_TSP_VectorTourReverse_Fixture, UpdaterCombine)->Apply(UpdaterCombineArgs);

BENCHMARK_DEFINE_F(BM_TSP_VectorTourReverse_Fixture, VectorReverse)(benchmark::State& st) {
    std::vector<Index> cities(tour.Size());

    double ratio =  st.range(1) / 100.;
    for (auto _ : st) {
        for (auto p : cityPairs) {
            Reverse(p.first, p.second, tour.Size(), [&](auto a, auto b) { std::swap(cities[a], cities[b]); }, ratio);
        }
    }
}

BENCHMARK_REGISTER_F(BM_TSP_VectorTourReverse_Fixture, VectorReverse)->Apply(UpdaterCombineArgs);


int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}