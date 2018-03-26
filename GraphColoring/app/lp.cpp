
#include "GC_LP.hpp"


int main(int argc, const char * argv[]) {

    std::string problem = "gc_20_7";
    std::string recoveryPath = "temp/" + problem;

    GC_LP solver(GC_LP_Rules::PerEdge);
    solver.set_max_seconds(10);
    solver.set_recovery_path(recoveryPath);

    ant::Stopwatch stopwatch;

    ifstream in("data/" + problem);
    auto gr = ReadProblem(in);
    auto c_gr = solver.solve(gr);

    Println(cout, "seconds passed: ", stopwatch.passed()/1000);

    if (!isFeasibleColoring(c_gr)) throw runtime_error("result is not feasible coloring");
    cout << problem << " : " << c_gr.colorCount() << endl;
    Println(cout, "used colors: ", c_gr.colors());
}