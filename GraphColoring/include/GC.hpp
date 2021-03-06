#pragma once

#include <iostream>

#include "ColoredGraph.hpp"


// all solvers should inherit this class
struct GC {
    virtual ColoredGraph solve(const Graph& gr) = 0;
    virtual string name() = 0;
    virtual ~GC() {}
};


void compareMethods(vector<unique_ptr<GC>>& methods) {
    Count node_count = 100;
    for (auto c = 0.5; c <= 0.6; c += 0.1) {
        Graph gr = graph::BuildRandom<graph::GraphBuilder<false, int16_t>>(node_count, c);
        for (auto& m : methods) {
            clock_t start = clock();
            int color_count = m->solve(gr).colorCount();
            clock_t finish = clock();
            cout << "name: " << m->name()
                 << "; colors:" << color_count
                 << ", time: " << finish - start << endl;
        }
    }
}


struct GC_Naive : GC {
    ColoredGraph solve(const Graph& gr) {
        // make number of colors approximation
        ColoredGraph cc(gr), &c_gr = cc;
        Color currentColor = 0; 
        while (!c_gr.uncoloredNodes().empty()) {
            auto uncoloredNodes = c_gr.uncoloredNodes();
            for (Node i : uncoloredNodes) {
               if (c_gr.adjacentNodesOfColorCount(i, currentColor) == 0) {
                   c_gr.setColor(i, currentColor);
               }
            }
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
        for (Node n = 0; n < c_gr.nodeCount(); ++n) {
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

struct GC_RecursiveLargestFirst : GC {
    ColoredGraph solve(const Graph& gr) {
        ColoredGraph c_gr(gr);

        Color currentColor = 0;
        auto i = MaxElement(c_gr.uncoloredNodes().begin(), c_gr.uncoloredNodes().end(), [&](auto n) {
            return c_gr.adjacentUncoloredNodeCount(n);
        });
        c_gr.setColor(*i, currentColor);

        while (c_gr.uncoloredNodeCount() > 0) {

            auto next = c_gr.uncoloredNodes()[0];
            auto nextDegree = 0;
            for (auto i : c_gr.uncoloredNodes()) {
                if (c_gr.adjacentNodesOfColorCount(i, currentColor) > 0) continue;

                auto degree = 0;
                for (auto j : c_gr.nextNodes(i)) {
                    degree += (c_gr.adjacentNodesOfColorCount(j, currentColor) > 0) ? 1 : 0;
                }
                if (degree > nextDegree) {
                    next = i;
                    nextDegree = degree;
                }
            }
            if (nextDegree > 0) {
                c_gr.setColor(next, currentColor);
            } else {

                ++currentColor;
                auto i = MaxElement(c_gr.uncoloredNodes().begin(), c_gr.uncoloredNodes().end(), [&](auto n) {
                    return c_gr.adjacentUncoloredNodeCount(n);
                });
                c_gr.setColor(*i, currentColor);

            }
        }
        return c_gr;
    }

    string name() {
        return "RecursiveLargestFirst";
    }
};