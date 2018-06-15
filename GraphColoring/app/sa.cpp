
#include "GC_SA.hpp"


int main(int argc, const char * argv[]) {

    std::string problem = "gc_100_7";
    std::string recoveryPath = "temp/" + problem;

    ifstream in("data/" + problem);
    auto gr = ReadProblem(in);

    const Count iterations = 1000000;

    CoolingSchedule_1 cooling(gr.nodeCount(), iterations);

    GC_SA solver(cooling, iterations);

    auto c_gr = solver.solve(gr);

    //Println(cout, solver.violations());
    Println(cout, solver.temperatures());

    if (!isFeasibleColoring(c_gr)) throw runtime_error("result is not feasible coloring");
    cout << problem << " : " << c_gr.colorCount() << endl;
    Println(cout, "used colors: ", c_gr.colors());
}