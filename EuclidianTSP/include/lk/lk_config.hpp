#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <range/v3/all.hpp>


struct LK_Config {

    double epsilon;
    Duration time_limit;
    Count max_perm_depth;
    Count neighbour_count;
    string perm;
};

std::ostream& operator<<(std::ostream& out, const LK_Config& config) {
    Println(out, "time_limit: ", FormatDuration(config.time_limit),
            " max_perm_depth: ", config.max_perm_depth);
    return out;
}

struct LK_Task {
    LK_Config config;
    std::string problem_name;
};

std::ostream& operator<<(std::ostream& out, const LK_Task& task) {
    Println(out, "task begin: ", task.problem_name, "\n", task.config, "task end: ", task.problem_name);
    return out;
}

class LK_ConfigParser {
    std::string problem_dir_;
    std::string result_dir_;

public:
    std::vector<LK_Task> Parse(const std::string& config_path) {

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(config_path, pt);

        problem_dir_ = pt.get<std::string>("problem_dir");
        result_dir_ = pt.get<std::string>("result_dir");

        std::vector<LK_Task> tasks;
        for (auto& v : pt.get_child("tasks"))
        {
            auto c = v.second;

            Duration time_limit = ParseDuration(c.get<std::string>("time_limit"));
            double epsilon = c.get<double>("epsilon");

            auto extract_values = ranges::view::transform([](auto& t) { return t.second.template get_value<Count>(); });

            auto max_perm_depth = c.get_child("max_perm_depth") | extract_values;
            auto neighbour_count = c.get_child("neighbour_count") | extract_values;

            auto problem = c.get_child("problem") |
                           ranges::view::transform([](auto& t) { return t.second.template get_value<std::string>(); });

            auto make_task = [&](Count max_perm_depth, Count neighbour_count, const std::string& problem) {
                return LK_Task{LK_Config{epsilon, time_limit, max_perm_depth, neighbour_count}, problem};
            };

            ranges::push_back(tasks, ranges::view::cartesian_product(max_perm_depth, neighbour_count, problem) |
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