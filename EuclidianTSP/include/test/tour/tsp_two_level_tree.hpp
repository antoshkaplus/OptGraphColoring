#pragma once

#include <gtest/gtest.h>

#include "tour/tsp_two_level_tree_base.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"


inline void CheckParentSync(const TwoLevelTreeBase& base) {
    base.ForEachParent([&](auto it) {
        ASSERT_TRUE(base.seg_ordered(base.seg_begin(it), base.seg_end(it)));

        auto count = 0;
        base.ForEach(base.seg_begin(it), base.seg_end(it), [&](auto city) {
            ++count;
            ASSERT_EQ(base.parent(city), it) << base;
        });
        ASSERT_EQ(count, it->size());
    });
}

inline auto Sequence(Count city_count) {
    std::vector<Index> vs(city_count);
    iota(vs.begin(), vs.end(), 0);
    return vs;
}

inline void Check(const TwoLevelTreeBase& base, const vector<Index>& expected_order) {
    CheckParentSync(base);
    auto start_order = std::find(expected_order.begin(), expected_order.end(), 0) - expected_order.begin();
    auto base_city = 0;
    auto next_city = base.Next(base_city);

    // take into account the whole sequence can be reversed
    auto diff = 0;
    if (expected_order[ant::next_ring_index(start_order, expected_order.size())] == next_city) diff =  1;
    if (expected_order[ant::prev_ring_index(start_order, expected_order.size())] == next_city) diff = -1;

    auto order = start_order + expected_order.size();
    for (auto i = 0; i < expected_order.size(); ++i) {
        ASSERT_EQ(base_city, expected_order[order % expected_order.size()]);
        base_city = base.Next(base_city);
        order += diff;
    }
}

inline void Check(const TwoLevelTreeTour& tour, const vector<Index>& expected_order) {
    Check(tour.base(), expected_order);
}