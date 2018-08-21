
#include <gtest/gtest.h>

#include "tour/tsp_two_level_tree_base.hpp"


void CheckParentSync(const TwoLevelTreeBase& base) {
    Index i = 0;
    base.ForEachParent([&](auto it) {
        ASSERT_EQ(i++, it->position());
        auto count = 0;
        base.ForEach(it, [&](auto city) {
            ++count;
            ASSERT_EQ(base.parent(city), it) << base;
        });
        ASSERT_EQ(count, it->size());
    });
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
    ASSERT_EQ(base.parent_count(), 1);
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
    {
        TwoLevelTreeBase base(6);
        base.SplitAt_2(1);
        base.SplitAt_2(3);
        base.SplitAt_2(5);
        base.Reverse(base.parent(1), base.parent(3));
        Check(base, {0, 4, 3, 2, 1, 5});
    }
}


TEST(TwoLevelTreeBase, Dereverse) {
    {
        TwoLevelTreeBase base(4);
        base.SplitAt_2(1);
        base.SplitAt_2(3);
        base.Reverse(base.parent(1));
        base.Dereverse(base.parent(1));
        Check(base, {0, 2, 1, 3});
    }
}

TEST(TwoLevelTreeBase, SplitReversed) {
    {
        TwoLevelTreeBase base(4);
        base.SplitAt_2(1);
        base.SplitAt_2(3);
        base.Reverse(base.parent(1));
        base.SplitAt_2(1);
        Check(base, {0, 2, 1, 3});
    }
}

TEST(TwoLevelTreeBase, MergeRight) {
    {
        TwoLevelTreeBase base(2);
        base.SplitAt_2(1);
        base.MergeRight(base.parent(0));
        Check(base, {0, 1});
        ASSERT_EQ(base.parent_count(), 1);
    }
    {
        TwoLevelTreeBase base(6);
        base.SplitAt_2(1);
        base.SplitAt_2(3);
        base.SplitAt_2(5);
        base.MergeRight(base.parent(1));
        Check(base, {0, 1, 2, 3, 4, 5});
        ASSERT_EQ(base.parent_count(), 3);
    }
    {
        TwoLevelTreeBase base(6);
        base.SplitAt_2(1);
        base.SplitAt_2(3);
        base.SplitAt_2(5);
        base.Reverse(base.parent(1));
        base.Reverse(base.parent(3));
        base.MergeRight(base.parent(1));
        Check(base, {0, 2, 1, 4, 3, 5});
        ASSERT_EQ(base.parent_count(), 3);
    }
    {
        TwoLevelTreeBase base(6);
        base.SplitAt_2(1);
        base.SplitAt_2(3);
        base.SplitAt_2(5);
        base.Reverse(base.parent(1));
        base.MergeRight(base.parent(1));
        Check(base, {0, 2, 1, 3, 4, 5});
        ASSERT_EQ(base.parent_count(), 3);
    }
}