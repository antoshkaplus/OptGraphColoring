#pragma once

#include <list>

#include <ant/core/core.hpp>
#include <ant/geometry/d2.hpp>

using namespace std;
using namespace ant;
using namespace ant::geometry::d2;
using namespace ant::geometry::d2::f;

using Problem = vector<Point>;
using Solution = vector<Index>;

inline Problem ReadProblem(istream& in) {
    Count point_count = 0;
    in >> point_count;
    Problem pr(point_count);
    for (auto& p : pr) {
        in >> p.x >> p.y;
    }
    return pr;
}

inline double TSP_Distance(const vector<Point>& pr, const vector<Index>& sol) {
    return accumulate(sol.begin(), sol.end(), make_pair(sol.back(), 0.), [&](auto pair, auto index) {
        return make_pair(index, pair.second + pr[index].Distance(pr[pair.first]));
    }).second;
}

inline void WriteProblem(ostream& out, const Problem& pr) {
    out << pr.size() << endl;
    for (auto p : pr) {
        out << p.x << " " << p.y << endl;
    }
}

inline void WriteSolution(ostream& out, const Solution& sol) {
    for (auto p : sol) {
        out << p << " ";
    }
}

inline bool isFeasibleSolution(const Problem& pr, const Solution& sol) {
    if (sol.size() != pr.size()) return false;
    if (unordered_set<Index>(sol.begin(), sol.end()).size() != sol.size()) return false;
    auto minmax = minmax_element(sol.begin(), sol.end());
    if (*minmax.first != 0 || *minmax.second != sol.size()-1) return false;

    return true;
}