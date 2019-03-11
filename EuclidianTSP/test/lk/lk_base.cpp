
#include <gtest/gtest.h>

#include "tour/tsp_vector_tour.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"
#include "lk/lk_base.hpp"
#include "lk/lk_permutation.hpp"
#include "lk/lk_permutation_v1.hpp"

template <typename Tour_Permutation>
class LK_BaseTest : public ::testing::Test {
public:
    using Tour = typename Tour_Permutation::first_type;
    using Permutation = typename Tour_Permutation::second_type;
};

TYPED_TEST_SUITE_P(LK_BaseTest);

using V = vector<Index>;

TYPED_TEST_P(LK_BaseTest, CanCloseMethod) {
    typename TestFixture::Tour tour(8);
    vector<Point> ps = { {0,0}, {2,0}, {4,0}, {4,2}, {4,4}, {2,4}, {0,4}, {0,2} };

    constexpr bool verbose = false;
    typename TestFixture::Permutation perm(tour);

    perm.Reset(0, 1);
    ASSERT_FALSE(perm.template CanClose<verbose>());

    perm.Assign(0, 1, 2, 3);
    ASSERT_FALSE(perm.template CanClose<verbose>());

    perm.Assign(0, 1, 5, 6);
    ASSERT_FALSE(perm.template CanClose<verbose>());

    perm.Assign(0, 1, 3, 2);
    ASSERT_TRUE(perm.template CanClose<verbose>());

    perm.Assign(3, 4, 7, 0, 5, 6);
    ASSERT_TRUE(perm.template CanClose<verbose>());

    perm.Assign(3, 2, 5, 6, 1, 0);
    ASSERT_TRUE(perm.template CanClose<verbose>());

    perm.Assign(3, 2, 5, 6, 0, 1);
    ASSERT_FALSE(perm.template CanClose<verbose>());
}

TYPED_TEST_P(LK_BaseTest, CloseMethod) {
    vector<Point> ps = { {0,0}, {2,0}, {4,0}, {4,2}, {4,4}, {2,4}, {0,4}, {0,2} };
    auto near = NearestNeighbours(ps, 5);

    using P = typename TestFixture::Permutation;
    {
        typename TestFixture::Tour tour(8);
        Close(tour, P{tour, 0, 1, 3, 2});
        Println(cout, "res: ", tour.Order());
        // should be something like 1 3 4 5 6 7 0 2
    }
    {
        typename TestFixture::Tour tour(8);
        Close(tour, P{tour, 3, 4, 7, 0, 5, 6});
        Println(cout, "res: ", tour.Order());
        // 1 2 3 6 7 4 5 0
    }
    {
        typename TestFixture::Tour tour(8);
        Close(tour, P{tour, 3, 2, 5, 6, 1, 0});
        Println(cout, "res: ", tour.Order());
        // 1 2 5 4 3 0 7 6
    }
    // result may have to be shifted or reversed fully
}

TYPED_TEST_P(LK_BaseTest, TryImproveMethod) {
    using T = typename TestFixture::Tour;
    using P = typename TestFixture::Permutation;

    vector<Point> ps = { {0,0}, {2,0}, {4,0}, {4,2}, {4,4}, {2,4}, {0,4}, {0,2} };
    auto near = NearestNeighbours(ps, 5);
    vector<Index> cities(8);
    iota(cities.begin(), cities.end(), 0);

    std::default_random_engine rng;
    for (auto i = 0; i < 100; ++i)
    {
        shuffle(cities.begin(), cities.end(), rng);
        typename TestFixture::Tour tour {cities};
        LK_Base<T, P> s(tour, ps, near, 1e-3);
        s.Solve();
        ASSERT_EQ(16, TSP_Distance(ps, tour.Order()));
    }
}

REGISTER_TYPED_TEST_SUITE_P(LK_BaseTest,
                            CanCloseMethod,
                            CloseMethod,
                            TryImproveMethod);

using LK_BaseTest_Types = ::testing::Types<
    std::pair<VectorTour, LK_Permutation_V1<VectorTour>>,
    std::pair<TwoLevelTreeTour, LK_Permutation_V1<TwoLevelTreeTour>>,
    std::pair<VectorTour, LK_Permutation<VectorTour>>>;

INSTANTIATE_TYPED_TEST_SUITE_P(My, LK_BaseTest, LK_BaseTest_Types);