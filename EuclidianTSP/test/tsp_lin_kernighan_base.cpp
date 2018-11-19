
#include <gtest/gtest.h>

#include "tour/tsp_vector_tour.hpp"
#include "tsp_lin_kernighan_base.hpp"


TEST(LinKernighanBase, CanClose) {
    VectorTour tour(8);
    vector<Point> ps = { {0,0}, {2,0}, {4,0}, {4,2}, {4,4}, {2,4}, {0,4}, {0,2} };
    auto near = NearestNeighbours(ps, 5);
    LinKernighanBase solver(tour, ps, near, 1e-3);

    constexpr bool verbose = false;
    ASSERT_FALSE(solver.CanClose<verbose>({0, 1}));
    ASSERT_FALSE(solver.CanClose<verbose>({0, 1, 2, 3}));
    ASSERT_FALSE(solver.CanClose<verbose>({0, 1, 5, 6}));
    ASSERT_TRUE(solver.CanClose<verbose>({0, 1, 3, 2}));

    ASSERT_TRUE(solver.CanClose<verbose>({3, 4, 7, 0, 5, 6}));
    ASSERT_TRUE(solver.CanClose<verbose>({3, 2, 5, 6, 1, 0}));
    ASSERT_FALSE(solver.CanClose<verbose>({3, 2, 5, 6, 0, 1}));
}

TEST(LinKernighanBase, Close) {
    vector<Point> ps = { {0,0}, {2,0}, {4,0}, {4,2}, {4,4}, {2,4}, {0,4}, {0,2} };
    auto near = NearestNeighbours(ps, 5);
    constexpr bool verbose = false;
    {
        VectorTour tour(8);
        LinKernighanBase s(tour, ps, near, 1e-3);
        s.Close<verbose>({0, 1, 3, 2});
        Println(cout, "res: ", tour.OrderToCity());
        // should be something like 1 3 4 5 6 7 0 2
    }
    {
        VectorTour tour(8);
        LinKernighanBase s(tour, ps, near,1e-3);
        s.Close<verbose>({3, 4, 7, 0, 5, 6});
        Println(cout, "res: ", tour.OrderToCity());
        // 1 2 3 6 7 4 5 0
    }
    {
        VectorTour tour(8);
        LinKernighanBase s(tour, ps, near,1e-3);
        s.Close<verbose>({3, 2, 5, 6, 1, 0});
        Println(cout, "res: ", tour.OrderToCity());
        // 1 2 5 4 3 0 7 6
    }

    // result may have to be shifted or reversed fully
}

TEST(LinKernighanBase, TryImprove) {
    vector<Point> ps = { {0,0}, {2,0}, {4,0}, {4,2}, {4,4}, {2,4}, {0,4}, {0,2} };
    auto near = NearestNeighbours(ps, 5);
    vector<Index> cities(8);
    iota(cities.begin(), cities.end(), 0);

    std::default_random_engine rng;
    for (auto i = 0; i < 100; ++i)
    {
        shuffle(cities.begin(), cities.end(), rng);
        VectorTour tour {cities};
        LinKernighanBase s(tour, ps, near, 1e-3);
        s.Solve();
        ASSERT_EQ(16, TSP_Distance(ps, tour.OrderToCity()));
    }
}

