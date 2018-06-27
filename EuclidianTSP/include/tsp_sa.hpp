#pragma once

#include <armadillo>

#include "tsp_tour.hpp"

class CoolingSchedule {
    double T_0 = 10000;
public:

    CoolingSchedule() {}
    CoolingSchedule(double T_0) : T_0(T_0) {}

    double temperature(Index iter) const {
        return T_0 / iter;
    }
};


class DistanceCompute {

    const std::vector<Point>& ps;

public:
    DistanceCompute(const std::vector<Point>& ps) : ps(ps) {}

    double operator()(Index i_1, Index i_2) const {
        return ps[i_1].Distance(ps[i_2]);
    }
};

class DistanceCache {

    arma::Mat<double> mat;

public:
    DistanceCache(const std::vector<Point>& ps) : mat(ps.size(), ps.size()) {
        grid::Region(0, 0, ps.size(), ps.size()).ForEach([&](auto pos) {
            mat(pos.row, pos.col) = ps[pos.row].Distance(ps[pos.col]);
        });
    }

    double operator()(Index i_1, Index i_2) const {
        return mat(i_1, i_2);
    }
};


template<class CoolingSchedule=::CoolingSchedule, class TDistance=DistanceCompute>
struct TSP_SA : TSP_Solver {

    // 100000
    Count iterations;

    TSP_SA(Count iterations) : iterations(iterations) {}


    vector<City> solve(const vector<Point>& points) {

        TDistance dist(points);

        vector<City> res(points.size());
        iota(res.begin(), res.end(), 0);
        default_random_engine rng;
        shuffle(res.begin(), res.end(), rng);


        uniform_int_distribution city_distr(0, Index(points.size()-1));
        for (auto iter = 0; iter < iterations; ++iter) {
            auto c_1 = city_distr(rng);
            auto c_2 = city_distr(rng);
            tie(c_1, c_2) = minmax(c_1, c_2);

            vector<City> new_route = res;

            reverse(new_route.begin()+c_1, new_route.begin()+c_2+1);

            // compute diff between distances

            // put it all in

            // what about trials on certain temperature ???

//            deriv.new_violations < coloring.violations ||
//            exp(-(deriv.new_violations - coloring.violations) / cooling_schedule.temperature(iter)) > zero_one_distr(rng)

            // Apply new route

            // keep result someplace else too
            // or we don't really need to
        }
    }

};