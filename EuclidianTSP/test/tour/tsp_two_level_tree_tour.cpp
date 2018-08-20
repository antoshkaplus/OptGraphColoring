
#include <gtest/gtest.h>

#include "tour/tsp_tour.hpp"
#include "tour/tsp_tour_reverse.hpp"
#include "tour/tsp_vector_tour.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"


TEST(TwoLevelTreeTour, Constructor) {
    for (auto i = 1; i < 10; ++i) {
        std::vector<Index> vs(10);
        iota(vs.begin(), vs.end(), 0);

        TwoLevelTreeTour tour(10);
        auto order = tour.Order();

        ASSERT_EQ(vs, order);
    }
}

TEST(TwoLevelTreeTour, Reverse) {
    for (auto n = 1; n <= 16; ++n) {
        std::vector<Index> vs(n);
        iota(vs.begin(), vs.end(), 0);

        TwoLevelTreeTour tour(n);
        for (auto i = 0; i < n; ++i) {
            for (auto j = i; j < n; ++j) {
                Println(cout, "before");
                Println(cout, vs);
                Println(cout, tour.Order());
                Println(cout, tour);

                auto a = vs[i];
                auto b = vs[j];

                Println(cout, i, "=>", a, " : ", j, "=>", b, " bucks:", tour.bucket_count());

                reverse(vs.begin()+i, vs.begin()+j+1);
                tour.Reverse(a, b);

                Println(cout, vs);
                Println(cout, tour.Order());
                Println(cout, tour);

                ASSERT_EQ(vs, tour.Order());
            }
        }
    }
}