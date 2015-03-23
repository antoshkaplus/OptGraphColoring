//
//  GC.h
//  GraphColoring
//
//  Created by Anton Logunov on 3/25/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __GraphColoring__GC__
#define __GraphColoring__GC__

#include <iostream>

#include "ColoredGraph.hpp"


// all solvers should inherit this class
struct GC {
    virtual ColoredGraph solve(const Graph& gr) = 0;
    virtual string name() = 0;
}; 



struct GC_Naive : GC {
    ColoredGraph solve(const Graph& gr) {
        // make number of colors approximation
        ColoredGraph cc(gr), &c_gr = cc;
        Color currentColor = 0; 
        while (!c_gr.uncoloredNodes().empty()) {
            set<Node> coloredThisTurn;
            for (Node i : c_gr.uncoloredNodes()) {
               bool feasibleColor = true;
               for (Node j : c_gr.adjacencyList[i]) {
                   if (coloredThisTurn.count(j)) {
                       feasibleColor = false;
                       break;
                   }
               }
               if (feasibleColor) {
                   coloredThisTurn.insert(i);
               }
            }
            for (Node i : coloredThisTurn) c_gr.setColor(i, currentColor);
            currentColor++;
        }
        return c_gr;
    }       
    
    string name() {
        return "naive";
    }
};


// better and simpler solution than previous one
struct GC_Naive_2 : GC {
    ColoredGraph solve(const Graph& gr) {
        ColoredGraph cc(gr), &c_gr = cc;
        Color currentColor = 0; 
        for (Node n = 0; n < c_gr.nodeCount; ++n) {
            for (Color c = 0; c <= currentColor; ++c) {
                if (c_gr.adjacentNodesOfColorCount(n, c) == 0) {
                    c_gr.setColor(n, c);
                    break;
                }
            }
            if (!c_gr.isColored(n)) {
                ++currentColor;
                c_gr.setColor(n, currentColor);
            }
        } 
        return c_gr;
    }
    
    string name() {
        return "naive_2";
    }

};


#endif /* defined(__GraphColoring__GC__) */





