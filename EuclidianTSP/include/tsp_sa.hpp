#pragma once

#include <armadillo>
#include <ant/grid/grid.hpp>
#include <ant/optimization/sa/cooling_schedule.hpp>
#include <ant/optimization/sa/history.hpp>

#include "tsp.hpp"
#include "tour/tsp_tour.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"

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


template<class TDistance=DistanceCompute>
struct TSP_SA {
private:
    using History = ant::opt::sa::History;
    using HistoryItem = ant::opt::sa::HistoryItem;

    const vector<Point>& points;
    // 100000
    uint64_t iterations;
    double alpha;
    std::chrono::duration<int> time_limit;

    History history_;

public:
    TSP_SA(const vector<Point>& points, uint64_t iterations, std::chrono::duration<int> time_limit)
        : points(points), iterations(iterations), time_limit(time_limit) {}

    vector<City> solveKeepHistory(Count history_item_count) {
        history_ = History(iterations, history_item_count);
        return solve<true>();
    }

    template <bool keepHistory = false>
    vector<City> solve() {

        TDistance dist(points);
        TwoLevelTreeTour tour(points.size());

        default_random_engine rng;
        uniform_int_distribution city_distr(0, Index(points.size()-1));
        uniform_real_distribution zero_one_distr {};

        auto start = std::chrono::system_clock::now();
        auto total = TSP_Distance(points, tour.Order());

        for (uint64_t iter = 0; iter < iterations && std::chrono::system_clock::now() - start < time_limit; ++iter) {

            opt::sa::PolynomialCooling schedule {alpha};
            double temperature = schedule.temperature(iter);

            for (auto trial = 0; trial < 2*trial_count(); ++trial) {

                auto c_1 = city_distr(rng);
                auto c_2 = city_distr(rng);

                if (!tour.LineOrdered(c_1, c_2) || tour.Prev(c_1) == c_2) {
                    continue;
                }

                auto c_1_prev = tour.Prev(c_1);
                auto c_2_next = tour.Next(c_2);

                double d_old = dist(c_1_prev, c_1) + dist(c_2, c_2_next);
                double d_new = dist(c_1, c_2_next) + dist(c_2, c_1_prev);

                double diff = d_new - d_old;

                if (diff < 0) {
                    tour.Reverse(c_1, c_2);
                    total += diff;
                    history_[iter].AddCost(total);
                } else {
                    double accept_prob = exp( -(diff) / (2 * total / points.size() * temperature) );
                    if constexpr (keepHistory) history_[iter].Add(total, temperature, accept_prob);

                    if (accept_prob > zero_one_distr(rng)) {
                        tour.Reverse(c_1, c_2);
                        total += diff;
                        history_[iter].AddCost(total);
                    }
                }
            }
        }
        return tour.Order();
    }

    Count trial_count() const {
        return points.size();
    }

    void set_alpha(double alpha) {
        this->alpha = alpha;
    }

    const ant::opt::sa::History& history() const {
        return history_;
    }
};