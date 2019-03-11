
#include <gtest/gtest.h>

#include "tsp_util.hpp"
#include "tour/tsp_vector_tour.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"
#include "lk/lk_permutation.hpp"
#include "lk/lk_permutation_v1.hpp"

template <typename Permutation>
class LK_PermutationTest : public ::testing::Test {
public:
};

TYPED_TEST_SUITE_P(LK_PermutationTest);

using V = vector<Index>;

TYPED_TEST_P(LK_PermutationTest, ExistsMethod) {
    typename TypeParam::tour_type tour(8);
    TypeParam perm(tour);

    perm.Assign(0, 1, 4, 5);
    for (auto city : {0, 1, 4, 5}) ASSERT_TRUE(perm.Exists(city));
    for (auto city : {2, 3, 6, 7}) ASSERT_FALSE(perm.Exists(city));

    perm.Assign(7, 0, 6, 5, 3, 4, 1, 2);
    for (auto city : ranges::view::ints(0, 8)) ASSERT_TRUE(perm.Exists(city));
}



REGISTER_TYPED_TEST_SUITE_P(LK_PermutationTest,
                            ExistsMethod);

using LK_PermutationTest_Types = ::testing::Types<
    LK_Permutation_V1<VectorTour>,
    LK_Permutation<VectorTour>>;

INSTANTIATE_TYPED_TEST_SUITE_P(My, LK_PermutationTest, LK_PermutationTest_Types);