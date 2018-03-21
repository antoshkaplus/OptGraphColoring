
#include "GC_Backtracking.hpp"
#include "GC_HillClimbing.hpp"
#include "GC_GA.hpp"


int main(int argc, const char * argv[]) {

    for (auto s : {"100_3", "100_5", "100_7"}) {

#ifdef NAIVE

        GC_Naive solver;

#endif
#ifdef BACKTRACK

        GC_Backtracking solver(std::chrono::seconds{10});

#endif
#ifdef HILL_CLIMBING

        GC_HillClimbing solver;

#endif
#ifdef GA

        GC_GA solver;
        solver.set_max_iteration_count(100000);

#endif
#ifdef REC_LARGE_FIRST

        GC_RecursiveLargestFirst solver;

#endif

        ifstream in(Format("data/gc_%s", s));
        auto gr = ReadProblem(in);
        auto c_gr = solver.solve(gr);
        if (!isFeasibleColoring(c_gr)) throw runtime_error("result is not feasible coloring");
        cout << s << " : " << c_gr.colorCount() << endl;
    }

}