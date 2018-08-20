
#include <gtest/gtest.h>

#include "tour/tsp_tour.hpp"
#include "tour/tsp_tour_reverse.hpp"
#include "tour/tsp_vector_tour.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"


TEST(UpdaterMatch, Updater_1) {

    Updater_1 updater_1;

    vector<Index> orderToCity = {0, 1, 2, 3, 4};
    VectorTour tour(orderToCity);

    Updater_1_Factory factory;
    tour.Reverse(3, 4, factory);
    ASSERT_EQ(tour.OrderToCity(), vector<Index>({0, 1, 2, 4, 3}));
    ASSERT_EQ(tour.CityToOrder(), vector<Index>({0, 1, 2, 4, 3}));

    tour.Reverse(2, 2, factory);
    // nothing should change
    ASSERT_EQ(tour.OrderToCity(), vector<Index>({0, 1, 2, 4, 3}));
    ASSERT_EQ(tour.CityToOrder(), vector<Index>({0, 1, 2, 4, 3}));

    tour.Reverse(0, 3, factory);
    ASSERT_EQ(tour.OrderToCity(), vector<Index>({3, 4, 2, 1, 0}));
    ASSERT_EQ(tour.CityToOrder(), vector<Index>({4, 3, 2, 0, 1}));
}

TEST(UpdaterMatch, Updater_2) {

    Updater_2 updater_2;

    vector<Index> orderToCity = {0, 1, 2, 3, 4};
    VectorTour tour(orderToCity);

    Updater_1_Factory factory;
    tour.Reverse(3, 4, factory);
    ASSERT_EQ(tour.OrderToCity(), vector<Index>({0, 1, 2, 4, 3}));
    ASSERT_EQ(tour.CityToOrder(), vector<Index>({0, 1, 2, 4, 3}));

    tour.Reverse(2, 2, factory);
    // nothing should change
    ASSERT_EQ(tour.OrderToCity(), vector<Index>({0, 1, 2, 4, 3}));
    ASSERT_EQ(tour.CityToOrder(), vector<Index>({0, 1, 2, 4, 3}));

    tour.Reverse(0, 3, factory);
    ASSERT_EQ(tour.OrderToCity(), vector<Index>({3, 4, 2, 1, 0}));
    ASSERT_EQ(tour.CityToOrder(), vector<Index>({4, 3, 2, 0, 1}));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
