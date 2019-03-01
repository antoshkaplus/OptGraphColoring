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

public:
    std::vector<SA_Task> Parse(const std::string& config_path) {

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(config_path, pt);

        std::vector<SA_Task> tasks;
        for (auto& v : pt.get_child("tasks"))
        {
            auto c = v.second;

            std::string cooling = c.get<std::string>("cooling");
            Duration time_limit = ParseDuration(c.get<std::string>("time_limit"));

            auto extract_values = ranges::view::transform([](auto& t) { return t.second.template get_value<double>(); });

            std::vector<double> trials_pr_sz_pow = c.get_child("trials_pr_sz_pow") | extract_values;
            //auto trials_pr_sz_pow = c.get_child("trials_pr_sz_pow") | extract_values;
            //auto alpha = c.get_child("alpha") | extract_values;
            std::vector<double> alpha = c.get_child("alpha") | extract_values;

            /*auto*/ vector<Count> no_improvement_iter_limit = c.get_child("no_improvement_iter_limit") |
                    ranges::view::transform([](auto& t) { return t.second.template get_value<Count>(); });

            /*auto*/ vector<std::string> problem = c.get_child("problem") |
                    ranges::view::transform([](auto& t) { return t.second.template get_value<std::string>(); });

            auto make_task = [&](double trials_pow, double alpha, Count iter_limit, const std::string& problem) {
                return SA_Task{SA_Config{cooling, time_limit, trials_pow, alpha, iter_limit}, problem};
            };

            std::vector<int> ps;

//            ranges::copy()

//            ranges::for_each(ranges::view::cartesian_product(trials_pr_sz_pow, alpha) |
//                             ranges::view::transform([](auto& t) -> int {return 0;}), std::back_inserter(ps));

            auto input = ranges::view::zip(trials_pr_sz_pow, alpha); // ranges::view::cartesian_product(trials_pr_sz_pow, alpha);
            std::function<int(const std::tuple<double, double>&)> f = [](const std::tuple<double, double>& t) -> int {return 0;};
            auto out = std::back_inserter(ps);
            ranges::transform(input,  out, f);

//            std::transform(std::begin(input), std::end(input), std::back_inserter(ps), f);


//            ranges::transform(ranges::view::cartesian_product(trials_pr_sz_pow, alpha, no_improvement_iter_limit, problem),
//                    std::back_inserter(tasks), [&](auto& tuple) { return std::apply(make_task, tuple); });
        }

        return tasks;
    }



};