#pragma once

#include <ctime>

#include <experimental/filesystem>

#include <boost/algorithm/string.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "report.hpp"

#include "sa/sa.hpp"
#include "sa/sa_config.hpp"

// uuid is created per problem
// print to file
// print to log all phase changes.
// we use boost logger as the fastest way to get it synchronized.

//void FindCoolingSchedule() {
//    ifstream in("data/tsp_33810_1");
//    auto problem = ReadProblem(in);
//    //auto iters = static_cast<Count>(std::pow(problem.size(), 1.2));
//
//    auto iters = 10000*10;
//
//    std::vector<double> power = {1.0, 1.2, 1.5};
//    std::vector<double> distance(3);
//
//    tbb::parallel_for(0, 3, [&](Index index) {
//        Println(cout, "started solving ", index);
//        TSP_SA solver(problem, iters, std::chrono::hours(1));
//        solver.set_alpha(power[index]);
//        auto solution = solver.solveKeepHistory(1000);
//        if (!isFeasibleSolution(problem, solution)) throw std::runtime_error("solution is not feasible " + std::to_string(index));
//        distance[index] = TSP_Distance(problem, solution);
//
//        Println(cout, "output result");
//        ofstream out("temp/history_tsp_33810_1_" + std::to_string(index));
//        for (auto& item : solver.history().items()) {
//            Println(out, item.best_cost, " ", item.accept_prob.min, " ", item.accept_prob.max, " ", item.temp.min, " ", item.temp.max);
//        }
//    });
//
//    for (auto d : distance) Println(cout, d);
//}

//void FindCoolingScheduleNew() {
//    ifstream in("data/tsp_1889_1");
//    auto problem = ReadProblem(in);
//    //auto iters = static_cast<Count>(std::pow(problem.size(), 1.2));
//
//
//    std::vector<Count> iters = {100, 120, 130};
//    std::vector<double> distance(3);
//
//    tbb::parallel_for(0, 3, [&](Index index) {
//        Println(cout, "started solving ", index);
//        TSP_SA solver(problem, iters[index], std::chrono::hours(1));
//        solver.set_alpha(0.9);
//        auto solution = solver.solveNew();
//        if (!isFeasibleSolution(problem, solution)) throw std::runtime_error("solution is not feasible " + std::to_string(index));
//        distance[index] = TSP_Distance(problem, solution);
//
//        Println(cout, "output result");
//        ofstream out("temp/history_tsp_33810_1_" + std::to_string(index));
//        for (auto& item : solver.history().items()) {
//            Println(out, item.best_cost, " ", item.accept_prob.min, " ", item.accept_prob.max, " ", item.temp.min, " ", item.temp.max);
//        }
//    });
//
//    for (auto d : distance) Println(cout, d);
//}



void Run_SA_Task(SA_Task task) {
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
}


void SA_Analyze(const std::string& config_path) {
    SA_ConfigParser parser;
    auto tasks = parser.Parse(config_path);

    auto problem_dir = parser.problem_dir();
    auto result_dir = parser.result_dir();

    // inside this dir I need to put log file with what's going on
    // also multiple files corresponding to running each configuration


    std::experimental::filesystem::path p(config_path);
    auto pp = p.filename().string();
    pp = report::EraseEnd(pp, ".json");
    pp += report::FormatNow("__%Y_%m_%d_%H_%M");

    std::experimental::filesystem::path analysis_path = result_dir;
    analysis_path /= pp;
    analysis_path = report::GetUnique(analysis_path);

    std::experimental::filesystem::create_directories(analysis_path);

    // we have now directory where we gonna store everything we encounter
    // now we have one log file where we log our progress
    // and we also do our jobs in parallel

    boost::log::add_file_log();

    boost::log::sources::logger_mt logger;

    tbb::parallel_for(tasks.begin(), tasks.end(), [&](SA_Task) {



    });
}