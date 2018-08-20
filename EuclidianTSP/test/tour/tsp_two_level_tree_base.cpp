
#include <gtest/gtest.h>

#include "tour/tsp_two_level_tree_base.hpp"


void CheckParentSync(const TwoLevelTreeBase& base) {
    auto& ps = base.parents;
    for (auto it = ps.begin(); it != ps.end(); ++it) {
        base.ForEach(it, [&](auto city) {
           ASSERT_EQ(base.elements[city].parent, it) << base;
        });
    }
}

auto Sequence(Count city_count) {
    std::vector<Index> vs(city_count);
    iota(vs.begin(), vs.end(), 0);
    return vs;
}

void Check(const TwoLevelTreeBase& base, const vector<Index>& expected_order) {
    CheckParentSync(base);
    ASSERT_EQ(base.Order(), expected_order) << base;
}



TEST(TwoLevelTreeBase, Constructor) {
    TwoLevelTreeBase base(10);
    ASSERT_EQ(base.parents.size(), 1);
    CheckParentSync(base);

    ASSERT_EQ(base.Order(), Sequence(10));
}


TEST(TwoLevelTreeBase, Split) {
    {
        TwoLevelTreeBase base(1);
        base.SplitAt_2(0);
        Check(base, {0});
    }
    {
        TwoLevelTreeBase base(2);
        base.SplitAt_2(1);
        Check(base, {0, 1});
    }
    {
        TwoLevelTreeBase base(3);
        base.SplitAt_2(1);
        Check(base, {0, 1, 2});
    }
}


TEST(TwoLevelTreeBase, Reverse_1) {
    {
        TwoLevelTreeBase base(1);
        base.Reverse(base.parent(0));
        Check(base, {0});
    }
    {
        TwoLevelTreeBase base(2);
        base.Reverse(base.parent(0));
        Check(base, {1, 0});
    }
    {
        TwoLevelTreeBase base(2);
        base.SplitAt_2(1);
        base.Reverse(base.parent(0));
        Check(base, {0, 1});
    }
    {
        TwoLevelTreeBase base(4);
        base.SplitAt_2(1);
        base.SplitAt_2(3);
        Check(base, {0, 1, 2, 3});
        base.Reverse(base.parent(1));
        Check(base, {0, 2, 1, 3});
    }
}


TEST(TwoLevelTreeBase, Reverse_2) {
    {
        TwoLevelTreeBase base(2);
        base.SplitAt_2(1);
        base.Reverse(base.parent(0), base.parent(1));
        Check(base, {1, 0});
    }
    {
        TwoLevelTreeBase base(4);
        base.SplitAt_2(1);
        base.SplitAt_2(2);
        base.SplitAt_2(3);
        base.Reverse(base.parent(1), base.parent(2));
        Check(base, {0, 2, 1, 3});
    }
}