
#include <gtest/gtest.h>

#include "tour/tsp_vector_tour.hpp"
#include "tsp_lin_kernighan_base.hpp"


TEST(LinKernighanBase, CanClose) {
    VectorTour tour(10);
    LinKernighanBase solver(tour, /* need to provide some points too */);

    {
        std::vector<Index> ts = {0, 1};
        ASSERT_FALSE(solver.CanClose(ts));
    }
}