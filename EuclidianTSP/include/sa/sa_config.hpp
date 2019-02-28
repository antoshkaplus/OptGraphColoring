#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


struct SA_Config {

    std::string cooling;
    std::chrono::duration<int64_t> time_limit;

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
    std::vector<SA_Config> Parse(const std::string& config_path) {

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(config_path, pt);

        std::vector<SA_Task> tasks;
        for (auto& v : pt.get_child("tasks"))
        {
            auto c = v.second;

            SA_Config config;
            config.cooling = c.get<std::string>("cooling");
            auto time_limit = c.get<std::string>("time_limit");
            // get integer + extension, make

            std::string cooling;
            std::chrono::duration<int64_t> time_limit;

            double trials_pr_sz_pow;
            double alpha;
            Count no_improvement_iter_limit;


            assert(v.first.empty()); // array elements have no names
            std::cout << v.second.data() << std::endl;
            // etc
        }


    }



};