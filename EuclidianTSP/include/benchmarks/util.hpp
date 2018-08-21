#pragma once

#include <ant/core/core.hpp>


using namespace ant;
using namespace std;

extern const std::array<Count, 6> kCityCounts;


inline vector<std::array<Index, 2>> cityPairs(Count city_count, Count pair_count) {
    default_random_engine rng;

    uniform_int_distribution distr(0, city_count-1);
    vector<std::array<Index, 2>> pairs(pair_count);
    for (auto& [p_1, p_2] : pairs) {
        p_1 = distr(rng);
        p_2 = distr(rng);
    }
    return pairs;
}