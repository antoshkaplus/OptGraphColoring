#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <range/v3/all.hpp>


struct SA_Config {

    std::string cooling;
    Duration time_limit;

    double trials_pr_sz_pow;
    double alpha;
    Count no_improvement_iter_limit;

};

struct SA_Task {
    SA_Config config;
    std::string problem_name;
};

class SA_ConfigParser {
    std::string problem_dir_;
    std::string result_dir_;

public:
    std::vector<SA_Task> Parse(const std::string& config_path) {

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(config_path, pt);

        problem_dir_ = pt.get<std::string>("problem_dir");
        result_dir_ = pt.get<std::string>("result_dir");

        std::vector<SA_Task> tasks;
        for (auto& v : pt.get_child("tasks"))
        {
            auto c = v.second;

            auto cooling = c.get<std::string>("cooling");
            Duration time_limit = ParseDuration(c.get<std::string>("time_limit"));

            auto extract_values = ranges::view::transform([](auto& t) { return t.second.template get_value<double>(); });

            auto trials_pr_sz_pow = c.get_child("trials_pr_sz_pow") | extract_values;
            auto alpha = c.get_child("alpha") | extract_values;

            auto no_improvement_iter_limit = c.get_child("no_improvement_iter_limit") |
                    ranges::view::transform([](auto& t) { return t.second.template get_value<Count>(); });

            auto problem = c.get_child("problem") |
                    ranges::view::transform([](auto& t) { return t.second.template get_value<std::string>(); });

            auto make_task = [&](double trials_pow, double alpha, Count iter_limit, const std::string& problem) {
                return SA_Task{SA_Config{cooling, time_limit, trials_pow, alpha, iter_limit}, problem};
            };

            ranges::push_back(tasks, ranges::view::cartesian_product(trials_pr_sz_pow, alpha, no_improvement_iter_limit, problem) |
                                     ranges::view::transform([&](const auto& tuple) { return std::apply(make_task, tuple); }));
        }

        return tasks;
    }

    const std::string& problem_dir() const {
        return problem_dir_;
    }

    const std::string& result_dir() const {
        return result_dir_;
    }

};