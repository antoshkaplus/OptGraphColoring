
#include <tbb/tbb.h>

#include "GC_GA.hpp"
#include "GC_GA_Original.hpp"

struct Result {
    ColorCount colorCount;
};

template<GC_GA_Flags flags>
void Compute(const std::string_view& name, const GC_GA_Params& params = {}) {
//    vector<NodeCount> nodeCounts = {50};
//    vector<int> completeness = {1, 3};

    vector<NodeCount> nodeCounts = {50, 70, 100, 250, 500};
    vector<int> completeness = {1, 3, 5, 7, 9};

    vector<pair<NodeCount, int>> problems;
    for (auto nodes : nodeCounts) {
        for (auto c : completeness) {
            problems.emplace_back(nodes, c);
        }
    }

    tbb::concurrent_vector<GC_GA<flags>> solvers(problems.size());
    for (auto& s : solvers) {
        s.set_max_iteration_count(5000);
        params.Set(s);
    }

    vector<ColoredGraph> results(problems.size());

    tbb::parallel_for<Index>(0, problems.size(), [&](auto index) {
        auto gr = ReadProblem(problems[index].first, problems[index].second);
        results[index] = solvers[index].solve(gr);
    });

    ofstream out(Format("scores/ga_compare_%.*s", name.size(), name.data()));
    for (auto i = 0; i < problems.size(); ++i) {
        Println(out, Format("%d %d %d", problems[i].first, problems[i].second, results[i].colorCount()));
    }
}

void ExploreMutationAtRandom() {

    tbb::parallel_invoke(std::bind(Compute<GC_GA_Flags::Mutation_1_Random|GC_GA_Flags::Mutation_2_Random>, "mut_random", GC_GA_Params()),
                         std::bind(Compute<GC_GA_Flags::Mutation_1_Random>, "mut_1_random", GC_GA_Params()),
                         std::bind(Compute<GC_GA_Flags::Mutation_2_Random>, "mut_2_random", GC_GA_Params()),
                         std::bind(Compute<GC_GA_Flags::None>, "none", GC_GA_Params()));
}

void ExplorePopulationSize() {
    vector<GC_GA_Params> params(3);
    params[0].population_size = 50;
    params[1].population_size = 70;
    params[2].population_size = 100;

    vector funcs = {
        std::bind(Compute<GC_GA_Flags::None>, "pop_50", params[0]),
        std::bind(Compute<GC_GA_Flags::None>, "pop_70", params[1]),
        std::bind(Compute<GC_GA_Flags::None>, "pop_100", params[2])
    };

    tbb::parallel_invoke(funcs[0], funcs[1], funcs[2]);
}

void ExploreCrossoverBoth() {

    Compute<GC_GA_Flags::CrossoverBoth>("cross", GC_GA_Params());

}

void ExploreMutationFrequency() {
    vector<pair<const char*, GC_GA_Params>> params(3);
    params[0].first = "freq_05";
    params[0].second.mutation_frequency = 0.5;

    params[1].first = "freq_07";
    params[1].second.mutation_frequency = 0.7;

    params[2].first = "freq_10";
    params[2].second.mutation_frequency = 1.;

    auto func = std::bind([](auto& params) { Compute<GC_GA_Flags::None>(params.first, params.second); }, std::placeholders::_1);
    tbb::parallel_for_each(params, func);
}

void ExploreCriticalFitness() {
    vector<pair<const char*, GC_GA_Params>> params(3);
    params[0].first = "fit_04";
    params[0].second.critical_fitness = 4;

    // best one
    params[1].first = "fit_06";
    params[1].second.critical_fitness = 6;

    params[2].first = "fit_10";
    params[2].second.critical_fitness = 10;

    auto func = std::bind([](auto& params) { Compute<GC_GA_Flags::None>(params.first, params.second); }, std::placeholders::_1);
    tbb::parallel_for_each(params, func);
}

void ExploreTopPortion() {

    vector<pair<const char*, GC_GA_Params>> params(3);
    params[0].first = "top_04";
    params[0].second.top_portion = 0.4;

    params[1].first = "top_05";
    params[1].second.top_portion = 0.5;

    params[2].first = "top_06";
    params[2].second.top_portion = 0.6;

    params[2].first = "top_07";
    params[2].second.top_portion = 0.7;

    auto func = std::bind([](auto& params) { Compute<GC_GA_Flags::None>(params.first, params.second); }, std::placeholders::_1);
    tbb::parallel_for_each(params, func);
}

int main() {
    // best result mut freq 1 and critical fitness 6
    GC_GA_Params params;
    params.critical_fitness = 6;
    params.mutation_frequency = 1;
    Compute<GC_GA_Flags::None>("total", params);

}
