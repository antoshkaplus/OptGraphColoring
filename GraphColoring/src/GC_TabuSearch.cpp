//
//  GC_TabuSearch.cpp
//  GraphColoring
//
//  Created by Anton Logunov on 3/28/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include "GC_TabuSearch.hpp"

ColoredGraph GC_TabuSearch::solve(const Graph& gr) {
    // how many iterations can't swap colors
    // trying out value of 10
    tabuList.tenure = 10;

    ColoredGraph result(gr);
    GC_Naive naive;
    ColoredGraph c_gr(naive.solve(gr));
    this->c_gr = &c_gr;

    while (c_gr.uncoloredNodeCount() == 0) {
        result = c_gr;
        // going from complete k+1 to partial k colored graph
        Color eliminatingColor = colorWithMinTotalDegree();
        for (Node i = 0; i < c_gr.nodeCount; i++) {
            if (c_gr.color(i) == eliminatingColor) c_gr.unsetColor(i);
        }
        tabuList.clear();
        currentIteration = 0;
        minUncoloredNodesTotalDegree = uncoloredNodesTotalDegree();
        while (c_gr.uncoloredNodeCount() > 0 &&
                currentIteration < 100000) {
            // choosing neighborhood
            Node i = randomUncoloredNode();
            localSearch(i);
            reduceUncoloredNodeCount();
            Degree d = uncoloredNodesTotalDegree();
            if (d < minUncoloredNodesTotalDegree) minUncoloredNodesTotalDegree = d;
            // should store minimum total degree here
            currentIteration++;
            if (currentIteration % 1000 == 0 
                && isVerbose()) {
                
                std::cout << currentIteration << endl;
            }
        }
    }
    return result;
}