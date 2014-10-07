 //
//  main.cpp
//  GraphColoring
//
//  Created by Anton Logunov on 6/20/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <set>

#include "GC_BB_NodeLevel.h"
#include "GC_TabuSearch.h"
#include "GC_GA.h"
#include "GC_Tabucol.h"

bool isFeasibleColoring(const ColoredGraph& c_gr) {
    if (c_gr.uncoloredNodeCount() > 0) return false;
    for (Node i = 0; i < c_gr.nodeCount; i++) {
        if (c_gr.adjacentNodesOfColorCount(i, c_gr.color(i)) > 0) return false;
    }
    return true;
}

void compareMethods() {
    GC_Tabucol tabucol;
    GC_Naive naive;
    GC_Naive_2 naive_2;
    GC_BB_NodeLevel bb_nodeLevel(GC_BB_NodeLevel::Duration(60));
    GC_TabuSearch tabuSearch;
    Count nodeCount = 1000;
    for (auto c = 0.5; c <= 0.6; c+=0.1) {
        Graph gr = Graph::random(nodeCount, c);
        for (GC* gc : vector<GC*>{&tabucol, &naive, &naive_2, /*&bb_nodeLevel,*/ &tabuSearch}) {
            clock_t start = clock();
            int colorCount = gc->solve(gr).colorCount();
            clock_t finish = clock();
            cout << typeid(*gc).name() << " : " << colorCount << " " << finish - start << endl;
        }
        cout << endl;
    }
}



int main(int argc, const char * argv[]) {
    if (argc == 1) {
        compareMethods();
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
    GC_TabuSearch gc;
    ColoredGraph c_gr = gc.solve(Graph(edges, n_nodes));
    if (!isFeasibleColoring(c_gr)) throw "fuck you, your method sucks!";
    solution = c_gr.coloring();
    cout << "\n\n\n";
    // output should here now on
    out << c_gr.colorCount() << " " << 0 << endl;
    for (int i = 0; i < solution.size(); i++) {
        out << solution[i] << " ";
    }
    return 0;
}

