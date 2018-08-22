#pragma once

#include <array>

#include <benchmark/benchmark.h>
#include <ant/core/core.hpp>


using namespace ant;
using namespace std;

extern const std::array<Count, 6> kCityCounts;


inline vector<std::array<Index, 2>> cityPairs(Count city_count, Count pair_count) {
    default_random_engine rng;

    uniform_int_distribution distr(0, city_count-1);
    vector<std::array<Index, 2>> pairs(pair_count);
    for (auto& p : pairs) {
        p[0] = distr(rng);
        p[1] = distr(rng);
    }
    return pairs;
}