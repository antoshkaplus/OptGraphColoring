
#include <tbb/tbb.h>

#include "GC_GA.hpp"
#include "GC_GA_Original.hpp"

struct Result {
    ColorCount colorCount;
};

template<GC_GA_Flags flags>
void Compute(std::string_view name) {
    vector<NodeCount> nodeCounts = {50};
    vector<int> completeness = {1, 3};

//    vector<NodeCount> nodeCounts = {50, 100, 500};
//    vector<int> completeness = {1, 3, 5, 7, 9};

    vector<pair<NodeCount, int>> problems;
    for (auto nodes : nodeCounts) {
        for (auto c : completeness) {
            problems.emplace_back(nodes, c);
        }
    }

    tbb::concurrent_vector<GC_GA<flags>> solvers(problems.size());
    vector<ColoredGraph> results(problems.size());

    tbb::parallel_for<Index>(0, problems.size(), [&](auto index) {
        auto gr = ReadProblem(problems[index].first, problems[index].second);
        results[index] = solvers[index].solve(gr);
    });

    ofstream out(Format("scores/ga_compare_%s", name));
    for (auto i = 0; i < problems.size(); ++i) {
        Println(out, Format("%d %d %d", problems[i].first, problems[i].second, results[i].colorCount()));
    }
}


int main() {

    Compute<GC_GA_Flags::Mutation_1_Random>("mut_1_random");
    Compute<GC_GA_Flags::Mutation_2_Random>("mut_2_random");
    Compute<GC_GA_Flags::None>("none");

}