#pragma once

#include "GC.hpp"


struct CoolingSchedule {
    double temperature(Index iteration) {
        return 0;
    }
};

struct CoolingSchedule_1 {
private:
    double beta_init = 0.98;
    double alpha_factor;



    double alpha() const {
        return (0.2 + alpha_factor) / 0.2;
    }

public:
    CoolingSchedule_1(Count node_count, Count iterations)
            : alpha_factor(static_cast<double>(node_count)/iterations) {}

    double temperature(Index iteration) {
        return 1. / (beta_init * pow(alpha(), iteration));
    }
};


template<typename CoolingSchedule, bool binary_search = false>
class GC_SA : GC {

    struct Coloring {
        std::vector<Color> colors;
        Count violations;
        Count color_count;
    };

    struct Derivative {
        Index node;
        Color new_color;
        Count new_violations;
    };

    const Graph* gr;
    std::uniform_int_distribution<> node_distr;
    std::default_random_engine rng;
    const Count iterations;

    CoolingSchedule cooling_schedule;
    std::vector<Count> violations_;
    std::vector<double> temperatures_;

public:
    GC_SA(Count iterations) : iterations(iterations) {}

    GC_SA(const CoolingSchedule& cooling, Count iterations)
            : iterations(iterations), cooling_schedule(cooling) {}

    ColoredGraph solve(const Graph& gr) override {
        this->gr = &gr;
        node_distr = std::uniform_int_distribution(0, gr.nodeCount()-1);

        GC_Naive_2 naive;
        auto res = naive.solve(gr);

        Coloring coloring;
        coloring.violations = 0;
        coloring.color_count = res.colorCount();
        coloring.colors.resize(res.nodeCount());
        for (auto i = 0; i < res.nodeCount(); ++i) {
            coloring.colors[i] = res.color(i);
        }

        if constexpr (binary_search) {

            // takes longer, should try to use partial binary search if use one at all
            LogicalBinarySearch::Min(2, coloring.color_count, [&,this](Count color_count) {
                auto res = Solve(gr, iterations, color_count);
                if (!res) return false;
                if (res.value().color_count < coloring.color_count) {
                    coloring = res.value();
                }
                return true;
            });

        } else {

            while (auto res = Solve(gr, iterations, coloring.color_count - 1)) {
                coloring = res.value();
            }

        }
        return {gr, coloring.colors};
    }

    string name() {
        return "SA";
    }

    const vector<Count>& violations() const {
        return violations_;
    }

    const vector<double>& temperatures() const {
        return temperatures_;
    }

private:

    std::optional<Coloring> Solve(const Graph& gr, Count iterations, Count color_count) {
        std::uniform_real_distribution<> zero_one_distr;

        auto coloring = RandomColoring(gr.nodeCount(), color_count);
        for (auto iter = 0; iter < iterations; ++iter) {
            // same as generate neighbour
            for (auto trial = 0; trial < temperature_trials(); ++trial) {

                auto deriv = GenDerivative(coloring);

                if (deriv.new_violations < coloring.violations ||
                    exp(-(deriv.new_violations - coloring.violations) / cooling_schedule.temperature(iter)) > zero_one_distr(rng)) {
                    Apply(coloring, deriv);
                }

                if (coloring.violations == 0) {
                    return {coloring};
                }

//                temperatures_.push_back(cooling_schedule.temperature(iter));
//                violations_.push_back(coloring.violations);
            }
        }

        return {};
    }

    Count temperature_trials() const {
        return 3.4 * gr->nodeCount();
    }

    Derivative GenDerivative(const Coloring& coloring) {
        Node n = node_distr(rng);
        Color c = coloring.colors[n];
        std::uniform_int_distribution color_distr(0, coloring.color_count-1);

        auto new_c = c;
        while ((new_c = color_distr(rng)) == c);

        auto new_violations = coloring.violations;
        gr->forEachNextNode(n, [&](Node next) {
            if (c == coloring.colors[next]) --new_violations;
            if (new_c == coloring.colors[next]) ++new_violations;
        });

        return {n, new_c, new_violations};
    }

    Coloring RandomColoring(Count node_count, Count color_count) {
        Coloring coloring;
        auto& colors = coloring.colors;
        colors.resize(node_count);

        std::uniform_int_distribution color_distr(0, color_count-1);
        std::generate(colors.begin(), colors.end(), std::bind(color_distr, rng));

        coloring.violations = CountViolations(*gr, coloring.colors);
        coloring.color_count = color_count;

        return coloring;
    }

    void Apply(Coloring& coloring, const Derivative& deriv) {
        coloring.colors[deriv.node] = deriv.new_color;
        coloring.violations = deriv.new_violations;
    }
};