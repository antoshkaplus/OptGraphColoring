#pragma once

#include "lk_base.hpp"
#include "lk_config.hpp"
#include "lk_interface.hpp"
#include "lk_permutation_v1.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"


struct LK_Value final : LK_Interface {

    TwoLevelTreeTour tour;
    grid::Grid<Index> neighbours;
    std::unique_ptr<LK_Interface> ptr;

    // like a template constructor
    LK_Value(const LK_Config& config, const std::vector<Point>& ps) : tour(ps.size()) {

        if (config.perm == "P") {
            Init<LK_Permutation<TwoLevelTreeTour>>(config, ps);
        } else if (config.perm == "P_V1") {
            Init<LK_Permutation_V1<TwoLevelTreeTour>>(config, ps);
        } else {
            throw std::runtime_error("cooling not supported: " + config.perm);
        }
    }

    vector<City> Solve() override {
        return ptr->Solve();
    }

    int64_t iter_count() const override {
        return ptr->iter_count();
    }

    const vector<Count>& close_count() const override {
        return ptr->close_count();
    }

    bool time_limit_reached() const override {
        return ptr->time_limit_reached();
    }

    Duration time_spent() const override {
        return ptr->time_spent();
    }

private:
    template<typename Permutation>
    void Init(const LK_Config& config, const std::vector<Point>& ps) {
        neighbours = NearestNeighbours(ps, config.neighbour_count);

        auto base = std::make_unique<LK_Base<TwoLevelTreeTour, Permutation>>(
            tour, ps, neighbours, config.epsilon);
        base->set_time_limit(config.time_limit);
        base->set_max_perm_depth(config.max_perm_depth);

        ptr = std::move(base);
    }
};