
#include "GC_LP.hpp"


void CompareRules(string_view problem_name, const Graph& problem, ostream& out) {
    for (auto use_heuristic : {true, false}) {
        GC_LP solver;
        solver.set_max_seconds(1000);
        solver.set_use_heuristic(use_heuristic);

        ant::Stopwatch stopwatch;
        auto c_gr = solver.solve(problem);

        Println(out, "problem: ", problem_name, ", colors: ", c_gr.colorCount());
        if (!isFeasibleColoring(c_gr)) Println(out, "result is not feasible coloring");
        Println(out, "heuristic: ", use_heuristic, ", seconds: ", solver.seconds_passed(), ", iterations: ", solver.iterations_passed(),
                ", my seconds: ", stopwatch.passed()/1000.);
    }
}

template<class CompareFunc>
void Compare(const vector<string>& problem_set, const string& out_name, CompareFunc&& func) {
    ofstream out("temp/" + out_name);
    for (auto problem_name : problem_set) {
        ifstream in("data/" + problem_name);
        auto gr = ReadProblem(in);

        func(problem_name, gr, out);
    }
}



int main(int argc, const char * argv[]) {

    vector<string> problem_set = {"gc_20_3", "gc_20_5", "gc_20_7"};
    //vector problem_set = {"gc_50_3", "gc_50_5", "gc_50_7"};
    Compare(problem_set, "compare_rules.txt", CompareRules);
}