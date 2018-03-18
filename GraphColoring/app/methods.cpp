
#include "GC_Backtracking.hpp"
#include "GC_GA.hpp"


int main(int argc, const char * argv[]) {

    for (auto s : {"100_3", "100_5", "100_7"}) {

#ifdef NAIVE

        GC_Naive solver;

#endif
#ifdef BACKTRACK

        GC_Backtracking solver(std::chrono::seconds{10});

#endif

        ifstream in(Format("data/gc_%s", s));
        auto gr = ReadProblem(in);
        auto c_gr = solver.solve(gr);
        cout << s << " : " << c_gr.colorCount() << " iterations: " << solver.iterations() << endl;
    }

}