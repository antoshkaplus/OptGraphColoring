
#include "GC_Backtracking.hpp"
#include "GC_TabuSearch.hpp"
#include "GC_Tabucol.hpp"
#include "GC_HillClimbing.hpp"
#include "GC_KempeChain.hpp"
#include "GC_GA.hpp"
#include "GC_LP.hpp"


int main(int argc, const char * argv[]) {

    for (auto s : {"100_3", "100_5", "100_7"}) {
//    for (auto s : {"500_3", "500_5", "500_7"}) {

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
//        GC_GA::Params params;

//        solver.set_max_iteration_count(10000);

#endif
#ifdef REC_LARGE_FIRST

        GC_RecursiveLargestFirst solver;

#endif
#ifdef TS

        GC_TabuSearch solver;
        solver.setMaxIteration(1000000);

#endif
#ifdef TABUCOL

        GC_Tabucol solver;

#endif
#ifdef KEMPE_CHAIN

        GC_KempeChain solver;

#endif
#ifdef LP

        GC_LP solver(GC_LP_Rules::PerEdge);

#endif

        ifstream in(Format("data/gc_%s", s));
        auto gr = ReadProblem(in);
        auto c_gr = solver.solve(gr);
        if (!isFeasibleColoring(c_gr)) throw runtime_error("result is not feasible coloring");
        cout << s << " : " << c_gr.colorCount() << endl;
    }

}
