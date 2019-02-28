#pragma once

#include "sa.hpp"
#include "sa_config.hpp"

struct SA_Value final : SA_Interface {

    std::unique_ptr<SA_Interface> ptr;

    // like a template constructor
    SA_Value(const SA_Config& config, const std::vector<Point>& ps)  {
        std::string cooling;
        std::chrono::duration time_limit;

        double trials_pr_sz_pow;
        double alpha;
        Count no_improvement_iter_limit;

        if (config.cooling == "polynomial") {
            opt::sa::PolynomialCooling cooling(config.alpha);
            Init<opt::sa::PolynomialCooling>(config, cooling, ps);
        } else if (config.cooling) {
            opt::sa::ExponentialMultiplicativeCooling cooling(1, config.alpha)
            Init<opt::sa::ExponentialMultiplicativeCooling>(config, cooling, ps);
        }
    }

    vector<City> Solve() override {
        return ptr->Solve();
    }

    vector<City> SolveKeepHistory(Count history_item_count) override {
        return ptr->SolveKeepHistory();
    }

    const ant::opt::sa::History& history() override {
        return ptr->history();
    }

private:
    template<typename CoolingSchedule>
    void Init(const SA_Config& config, CoolingSchedule cooling, const std::vector<Point>& ps) {
        ptr.reset(new TSP_SA<CoolingSchedule>(ps, config.no_improvement_iter_limit,
            std::pow(ps.size(), config.trials_pr_sz_pow), config.time_limit));
    }
};