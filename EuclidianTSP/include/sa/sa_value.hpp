#pragma once

#include "sa.hpp"
#include "sa_config.hpp"

struct SA_Value final : SA_Interface {

    std::unique_ptr<SA_Interface> ptr;

    // like a template constructor
    SA_Value(const SA_Config& config, const std::vector<Point>& ps)  {
        if (config.cooling == "polynomial") {
            opt::sa::PolynomialCooling cooling(config.alpha);
            Init<opt::sa::PolynomialCooling>(config, cooling, ps);
        } else if (config.cooling == "exponential") {
            opt::sa::ExponentialMultiplicativeCooling cooling(1, config.alpha);
            Init<opt::sa::ExponentialMultiplicativeCooling>(config, cooling, ps);
        } else {
            throw std::runtime_error("cooling not supported: " + config.cooling);
        }
    }

    vector<City> Solve() override {
        return ptr->Solve();
    }

    vector<City> SolveKeepHistory(Count history_item_count) override {
        return ptr->SolveKeepHistory(history_item_count);
    }

    const ant::opt::sa::History& history() const override {
        return ptr->history();
    }

    virtual bool time_limit_reached() const override {
        return ptr->time_limit_reached();
    }

private:
    template<typename CoolingSchedule>
    void Init(const SA_Config& config, CoolingSchedule cooling, const std::vector<Point>& ps) {
        ptr.reset(new TSP_SA(ps, config.no_improvement_iter_limit,
            static_cast<int64_t>(std::pow(ps.size(), config.trials_pr_sz_pow)), config.time_limit, cooling));
    }
};