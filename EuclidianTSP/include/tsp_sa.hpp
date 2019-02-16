#pragma once

#include <armadillo>
#include <ant/grid/grid.hpp>

#include "tsp.hpp"
#include "tour/tsp_tour.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"

inline uint64_t TSP_SA_Iterations(uint64_t city_count) {
    return city_count * city_count * 100;
}

class CoolingSchedule {
    double T_0 = 10000;
public:

    CoolingSchedule() {}
    CoolingSchedule(double T_0) : T_0(T_0) {}

    double temperature(double iter) const {
        return T_0 / iter;
    }
};


class DistanceCompute {

    const std::vector<Point>& ps;

public:
    DistanceCompute(const std::vector<Point>& ps) : ps(ps) {}

    double operator()(Index i_1, Index i_2) const {
        return ps[i_1].Distance(ps[i_2]);
    }
};

class DistanceCache {

    arma::Mat<double> mat;

public:
    DistanceCache(const std::vector<Point>& ps) : mat(ps.size(), ps.size()) {
        grid::Region(0, 0, ps.size(), ps.size()).ForEach([&](auto pos) {
            mat(pos.row, pos.col) = ps[pos.row].Distance(ps[pos.col]);
        });
    }

    double operator()(Index i_1, Index i_2) const {
        return mat(i_1, i_2);
    }
};


template<class CoolingSchedule=::CoolingSchedule, class TDistance=DistanceCompute>
struct TSP_SA : TSP_Solver {

    // 100000
    uint64_t iterations;
    std::chrono::duration<int> time_limit;

    TSP_SA(uint64_t iterations, std::chrono::duration<int> time_limit) : iterations(iterations), time_limit(time_limit) {}


    vector<City> solve(const vector<Point>& points) {

        CoolingSchedule cooling_schedule(iterations);
        TDistance dist(points);
        TwoLevelTreeTour tour(points.size());

        default_random_engine rng;
        uniform_int_distribution city_distr(0, Index(points.size()-1));
        uniform_real_distribution zero_one_distr {};

        auto start = std::chrono::system_clock::now();
        for (uint64_t iter = 0; iter < iterations && std::chrono::system_clock::now() - start < time_limit; ++iter) {
            auto c_1 = city_distr(rng);
            auto c_2 = city_distr(rng);

            if (c_1 == c_2) continue;
            if (!tour.LineOrdered(c_1, c_2)) swap(c_1, c_2);

            auto c_1_prev = tour.Prev(c_1);
            auto c_2_next = tour.Next(c_2);

            double d_old = dist(c_1_prev, c_1) + dist(c_2, c_2_next);
            double d_new = dist(c_1, c_2_next) + dist(c_2, c_1_prev);

            double diff = d_new - d_old;

            if (diff < 0 || exp( -diff / cooling_schedule.temperature(iter) ) > zero_one_distr(rng)) {
                tour.Reverse(c_1, c_2);
            }
        }
        return tour.Order();
    }
};