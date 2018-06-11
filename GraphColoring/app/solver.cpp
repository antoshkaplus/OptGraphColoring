
#include "GC_Backtracking.hpp"
#include "GC_GA.hpp"


int main(int argc, const char * argv[]) {
    if (argc == 1) {
        vector<unique_ptr<GC>> ms;
        ms.emplace_back(new GC_GA<GC_GA_Flags::None>());
        compareMethods(ms);
        return 0;
    }
    string input(argv[1]);
    string output(argv[2]);

    cout << "input file: " << input << endl;
    cout << "output file: " << output << endl;

    ifstream in(input);
    ofstream out(output);

    vector<Color> solution;
    size_t n_nodes, n_edges;
    in >> n_nodes >> n_edges;
    vector<Edge> edges(n_edges);
    int k1, k2;
    for (int i = 0; i < n_edges; i++) {
        in >> k1 >> k2;
        edges[i] = Edge(k1, k2);
    }
    cout << "finding good approximation\n";
    //GC_TabuSearch gc;
//    ColoredGraph c_gr = gc.solve(Graph(edges, n_nodes));
//    if (!isFeasibleColoring(c_gr)) throw "fuck you, your method sucks!";
//    solution = c_gr.coloring();
//    cout << "\n\n\n";
//    // output should here now on
//    out << c_gr.colorCount() << " " << 0 << endl;
//    for (int i = 0; i < solution.size(); i++) {
//        out << solution[i] << " ";
//    }
    return 0;
}

