#pragma once

#include <gtest/gtest.h>

#include "tour/tsp_two_level_tree_base.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"


inline void CheckParentSync(const TwoLevelTreeBase& base) {
    Index i = 0;
    base.ForEachParent([&](auto it) {
        ASSERT_TRUE(base.seg_ordered(base.seg_begin(it), base.seg_end(it)));

        ASSERT_EQ(i++, it->position());
        auto count = 0;
        base.ForEach(it, [&](auto city) {
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
    ASSERT_EQ(base.Order(), expected_order) << base;
}

inline void Check(const TwoLevelTreeTour& tour, const vector<Index>& expected_order) {
    Check(tour.base(), expected_order);
}