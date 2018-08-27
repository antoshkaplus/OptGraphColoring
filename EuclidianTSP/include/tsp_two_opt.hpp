#pragma once

#include "tsp.hpp"


struct TSP_TwoOpt /*: TSP_Improver*/ {
    typedef size_t Index;

    double swap_distance_factor;

    TSP_TwoOpt() : swap_distance_factor(0.001) {}

    TSP_TwoOpt(double swap_distance_factor) {
        this->swap_distance_factor = swap_distance_factor;
    }

    // hard to copy tours right now
    // creating new may require certain parameters
    template<class Tour>
    vector<City> improve(const vector<Point>& ps, Tour& tour) {

        std::uniform_int_distribution<> distr(0, ps.size()-1);
        std::default_random_engine rng;

        auto tour_distance = TSP_Distance(ps, tour.Order());
        auto epsilon = swap_distance_factor * tour_distance;

        start:

        auto first = distr(rng);
        auto a1 = first;

        for (auto i = 0; i < ps.size(); ++i) {
            auto a2 = tour.Next(a1);
            auto b1 = tour.Next(a2);
            auto b2 = tour.Next(b1);

            while (b2 != first) {
                if (TSP_Distance(ps, a1, a2) + TSP_Distance(ps, b1, b2) > TSP_Distance(ps, a1, b1) + TSP_Distance(ps, a2, b2) + epsilon) {
                    tour.Flip(a1, a2, b1, b2);
                    goto start;
                }
                b1 = b2;
                b2 = tour.Next(b1);
            }

            a1 = a2;
        }

        return tour.Order();
    }
};
