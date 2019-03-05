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

    Duration time_limit;

    CoolingSchedule cooling_schedule;
    default_random_engine rng;

    History history_;
    Duration time_spent_;
    bool time_limit_reached_ = false;


public:
    TSP_SA(const vector<Point>& points, uint64_t no_improvement_iter_limit, uint64_t trials,
           Duration time_limit, const CoolingSchedule& cooling_schedule)

           : points(points), no_improvement_iter_limit(no_improvement_iter_limit),
             trials(trials), time_limit(time_limit), cooling_schedule(cooling_schedule) {}

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

    bool time_limit_reached() const override {
        return time_limit_reached_;
    }

    Duration time_spent() const override {
        return time_spent_;
    }

private:
    template <bool keepHistory = false>
    vector<City> solve() {

        TDistance dist(points);

        TwoLevelTreeTour tour = Init(points.size());

        uniform_int_distribution city_distr(0, Index(points.size()-1));
        uniform_real_distribution zero_one_distr {};

        auto saved_order = tour.Order();
        auto saved_total = TSP_Distance(points, saved_order);
        auto total = saved_total;
        auto best_total = total;

        uint64_t iterations = no_improvement_iter_limit;

        Timer timer(time_limit);
        auto start = std::chrono::steady_clock::now();

        for (uint64_t iter = 0; iter < iterations && !timer.timeout(); ++iter) {

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

                    tour.Reverse(c_1, c_2);
                    total += diff;

                    if (total < best_total) {
                        best_total = total;
                        iterations = iter + no_improvement_iter_limit + 1;
                        if constexpr (keepHistory) history_.IncreaseIterations(iterations);
                    }

                    if constexpr (keepHistory) history_[iter].AddCost(total);
                } else {
                    double accept_prob = exp( -(diff) / (2 * total / points.size() * temperature) );
                    if constexpr (keepHistory) history_[iter].Add(total, temperature, accept_prob);

                    if (accept_prob > zero_one_distr(rng)) {

                        if (total < saved_total) {
                            // from here saved total equals best_total
                            assert(best_total == total);
                            saved_total = total;
                            saved_order = tour.Order();
                        }

                        tour.Reverse(c_1, c_2);
                        total += diff;
                        if constexpr (keepHistory) history_[iter].AddCost(total);
                    }
                }
            }
        }

        if (total < saved_total) {
            saved_total = total;
            saved_order = tour.Order();
        }

        if (timer.timeout()) {
            time_limit_reached_ = true;
        }

        time_spent_ = std::chrono::steady_clock::now() - start;

        return saved_order;
    }

    TwoLevelTreeTour Init(Count problem_size) {
        std::vector<City> order(problem_size);
        std::iota(order.begin(), order.end(), 0);
        std::shuffle(order.begin(), order.end(), rng);
        return TwoLevelTreeTour{order};
    }

};