
#include <tbb/tbb.h>

#include "GC_GA.hpp"
#include "GC_GA_Original.hpp"

struct Result {
    ColorCount colorCount;
};

int main() {

    vector<NodeCount> nodeCounts = {50, 100, 500};
    vector<int> completeness = {1, 3, 5, 7, 9};

    vector<pair<NodeCount, int>> problems;
    for (auto nodes : nodeCounts) {
        for (auto c : completeness) {
            problems.emplace_back(nodes, c);
        }
    }

    tbb::concurrent_vector<GC_GA> solvers(problems.size());
    vector<ColoredGraph> results(problems.size());

    tbb::parallel_for<Index>(0, problems.size(), [&](auto index) {
         auto gr = ReadProblem(problems[index].first, problems[index].second);
         results[index] = solvers[index].solve(gr);
    });

    // now I need to figure out how to accumulate results
    auto name = "temp";
    ofstream out(Format("score/ga_compare_%s", name));
    for (auto i = 0; i < problems.size(); ++i) {
        Println(out, Format("%d %d %d", problems[i].first, problems[i].second, results[i].colorCount()));
    }
}