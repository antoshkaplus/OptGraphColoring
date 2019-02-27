
#include <tbb/parallel_do.h>
#include <tbb/parallel_for_each.h>

#include "tsp_util.hpp"
#include "tsp_sa.hpp"


// uuid is created per problem
// print to file
// print to log all phase changes.
// we use boost logger as the fastest way to get it synchronized.



struct SA_Config {

};


class SA_ConfigParser {


    std::vector<SA_Config> Parse() {

    }



};





void FindIterationCount() {
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

        TSP_SA solver(problem, iters, std::chrono::hours(8));
        info.solution = TSP_Distance(problem, solver.solve());
        Println(cout, "finished: ", names[info.problem], " ", info.multiplier, " ", info.solution);
    });

    for (auto& f : info) {
        Println(cout, "pr: ", names[f.problem], " , mult: ", std::to_string(f.multiplier), " , sol: ", f.solution);
    }
}


void FindCoolingSchedule() {
    ifstream in("data/tsp_33810_1");
    auto problem = ReadProblem(in);
    //auto iters = static_cast<Count>(std::pow(problem.size(), 1.2));

    auto iters = 10000*10;

    std::vector<double> power = {1.0, 1.2, 1.5};
    std::vector<double> distance(3);

    tbb::parallel_for(0, 3, [&](Index index) {
        Println(cout, "started solving ", index);
        TSP_SA solver(problem, iters, std::chrono::hours(1));
        solver.set_alpha(power[index]);
        auto solution = solver.solveKeepHistory(1000);
        if (!isFeasibleSolution(problem, solution)) throw std::runtime_error("solution is not feasible " + std::to_string(index));
        distance[index] = TSP_Distance(problem, solution);

        Println(cout, "output result");
        ofstream out("temp/history_tsp_33810_1_" + std::to_string(index));
        for (auto& item : solver.history().items()) {
            Println(out, item.best_cost, " ", item.accept_prob.min, " ", item.accept_prob.max, " ", item.temp.min, " ", item.temp.max);
        }
    });

    for (auto d : distance) Println(cout, d);
}

void FindCoolingScheduleNew() {
    ifstream in("data/tsp_1889_1");
    auto problem = ReadProblem(in);
    //auto iters = static_cast<Count>(std::pow(problem.size(), 1.2));


    std::vector<Count> iters = {100, 120, 130};
    std::vector<double> distance(3);

    tbb::parallel_for(0, 3, [&](Index index) {
        Println(cout, "started solving ", index);
        TSP_SA solver(problem, iters[index], std::chrono::hours(1));
        solver.set_alpha(0.9);
        auto solution = solver.solveNew();
        if (!isFeasibleSolution(problem, solution)) throw std::runtime_error("solution is not feasible " + std::to_string(index));
        distance[index] = TSP_Distance(problem, solution);

        Println(cout, "output result");
        ofstream out("temp/history_tsp_33810_1_" + std::to_string(index));
        for (auto& item : solver.history().items()) {
            Println(out, item.best_cost, " ", item.accept_prob.min, " ", item.accept_prob.max, " ", item.temp.min, " ", item.temp.max);
        }
    });

    for (auto d : distance) Println(cout, d);
}



int main(int argc, const char * argv[]) {
    FindCoolingScheduleNew();
}
