
#include <gtest/gtest.h>

#include "support.h"
#include "branch_bound_fraction.h"
#include "branch_bound.h"
#include "lp.hpp"

TEST(Knapsack, Methods) {
    auto path = "data/ks_19_0";
    std::ifstream in(path);
    auto pr = ReadProblem(in);

    KS_BranchBound knapsack;
    KS_BranchBoundFraction knapsackOpt;
    KS_LP knapsack_LP;

    std::vector solutions = {
        knapsack.relaxation(pr.items, pr.capacity),
        knapsackOpt.relaxation(pr.items, pr.capacity),
        usual(pr.items, pr.capacity),
        knapsack_LP.solve(pr.items, pr.capacity)
    };

    std::vector<int> ps;
    for (auto& s : solutions) {
        auto v = Value(pr, s.second);
        ASSERT_EQ(s.first, v);
    }
    for (auto i = 1; i < solutions.size(); ++i) {
        ASSERT_EQ(solutions[i-1].first, solutions[i].first);
    }

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
