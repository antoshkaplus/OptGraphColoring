#pragma once

#include <armadillo>
#include <ant/grid/grid.hpp>
#include <ant/optimization/sa/cooling_schedule.hpp>
#include <ant/optimization/sa/history.hpp>

#include "tsp.hpp"
#include "tour/tsp_tour.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"

#include "sa_interface.hpp"

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


template<class CoolingSchedule, class TDistance=DistanceCompute>
struct TSP_SA final : SA_Interface {
private:
    using History = ant::opt::sa::History;
    using HistoryItem = ant::opt::sa::HistoryItem;

    const vector<Point>& points;

    uint64_t no_improvement_iter_limit;
    uint64_t trials;

    std::chrono::duration<int64_t> time_limit;

    CoolingSchedule cooling_schedule;

    History history_;

public:
    TSP_SA(const vector<Point>& points, uint64_t no_improvement_iter_limit, uint64_t trials,
           std::chrono::duration<int64_t> time_limit)

           : points(points), no_improvement_iter_limit(no_improvement_iter_limit), trials(trials), time_limit(time_limit) {}

    vector<City> SolveKeepHistory(Count history_item_count) override {
        history_ = History(no_improvement_iter_limit, history_item_count);
        return solve<true>();
    }

    vector<City> Solve() override {
        return solve<false>();
    }

    const ant::opt::sa::History& history() const override {
        return history_;
    }

    void set_cooling_schedule(CoolingSchedule& cooling_schedule) {
        this->cooling_schedule = cooling_schedule;
    }

private:
    template <bool keepHistory = false>
    vector<City> solve() {

        TDistance dist(points);
        TwoLevelTreeTour tour(points.size());

        default_random_engine rng;
        uniform_int_distribution city_distr(0, Index(points.size()-1));
        uniform_real_distribution zero_one_distr {};

        auto start = std::chrono::system_clock::now();
        auto total = TSP_Distance(points, tour.Order());

        uint64_t iterations = no_improvement_iter_limit;

        for (uint64_t iter = 0; iter < iterations && std::chrono::system_clock::now() - start < time_limit; ++iter) {

            auto temperature = cooling_schedule.temperature(iter);
            for (auto trial = 0; trial < 2*trials; ++trial) {

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
                    iterations = iter + no_improvement_iter_limit;
                    if constexpr (keepHistory) history_.IncreaseIterations(iterations);

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
};