#pragma once

#include <ctime>

#include <experimental/filesystem>

#include "report.hpp"

#include "lk_config.hpp"
#include "lk_value.hpp"


void Run_LK_Task(std::string_view task_id, report::Logger& logger, const LK_Task& task, std::string_view problem_dir, std::string_view result_dir) {
    logger.Log(task_id, " start: ", task);

    std::experimental::filesystem::path p{problem_dir};
    p /= task.problem_name;
    std::ifstream in(p);

    auto problem = ReadProblem(in);

    logger.Log(task_id, " solving");

    LK_Value solver(task.config, problem);

    auto solution = solver.Solve();

    if (!isFeasibleSolution(problem, solution)) {
        throw std::runtime_error(std::string("solution is not feasible ") + std::string(task_id));
    }
    auto distance = TSP_Distance(problem, solution);

    logger.Log(task_id,
               " result: ", distance,
               " spent: ", FormatDuration(solver.time_spent()),
               solver.time_limit_reached() ? " time_limit" : "");

    p = result_dir;
    p /= task_id;
    ofstream out(p);
    Println(out, task);
    Println(out, "iterations: ", solver.iter_count());
    Println(out, "permutations: ", solver.close_count());
    Println(out, "time: ", FormatDuration(solver.time_spent()));
    Println(out, "result: ", distance);
    logger.Log(task_id, " reported");
}


void LK_Analyze(const std::string& config_path) {
    LK_ConfigParser parser;
    auto tasks = parser.Parse(config_path);

    auto problem_dir = parser.problem_dir();
    auto result_dir = parser.result_dir();

    std::experimental::filesystem::path p(config_path);
    auto pp = p.filename().string();
    pp = report::EraseEnd(pp, ".json");
    pp += report::FormatNow("__%Y_%m_%d_%H_%M");

    std::experimental::filesystem::path analysis_path = result_dir;
    analysis_path /= pp;
    analysis_path = report::GetUnique(analysis_path);

    std::experimental::filesystem::create_directories(analysis_path);

    auto log_path = analysis_path;
    log_path /= "log";

    report::Logger logger(log_path);

    std::atomic_int task_id = 0;
    tbb::parallel_for_each(tasks.begin(), tasks.end(), [&](const LK_Task& task) {
        auto task_id_str = std::to_string(task_id++) + "_" + task.config.perm + "_" + task.problem_name;
        try {
            Run_LK_Task(task_id_str, logger, task, problem_dir, analysis_path.string());
        } catch (std::exception& ex) {
            logger.Log(task_id_str, " failed");
        }
    });
}