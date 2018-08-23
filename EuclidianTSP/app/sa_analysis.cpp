
#include <tbb/parallel_do.h>
#include <tbb/parallel_for_each.h>

#include "tsp_util.hpp"
#include "tsp_sa.hpp"



int main(int argc, const char * argv[]) {

    vector<string> names = {"tsp_400_1", "tsp_1400_1", "tsp_4461_1", "tsp_14051_1", "tsp_33810_1"};
    vector<Problem> problems;
    for (auto& n : names) {
        ifstream in(std::string("data/") + n);
        problems.emplace_back(ReadProblem(in));

        assert(problems.back().size() != 0);
    }
    std::vector<double> multipliers = {0.1, 1., 10., 100.};

    Println(cout, "read problems");

    struct Info {
        double multiplier;
        double solution;
        Index problem;

        Info() {}
        Info(Index problem, double multiplier)
            : multiplier(multiplier), problem(problem) {}
    };

    vector<Info> info;
    for (auto i = 0; i < names.size(); ++i) {
        for (auto m : multipliers) {
            info.emplace_back(i, m);
        }
    }

    tbb::parallel_do(info.begin(), info.end(), [&](Info& info) {
        auto& problem = problems[info.problem];

        uint64_t iters = problem.size() * problem.size() * info.multiplier;

        TSP_SA solver(iters, std::chrono::hours(8));
        info.solution = TSP_Distance(problem, solver.solve(problem));
        Println(cout, "finished: ", names[info.problem], " ", info.multiplier, " ", info.solution);
    });

    for (auto& f : info) {
        Println(cout, "pr: ", names[f.problem], " , mult: ", std::to_string(f.multiplier), " , sol: ", f.solution);
    }
}
