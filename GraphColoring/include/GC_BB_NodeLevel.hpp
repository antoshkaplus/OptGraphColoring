//
//  BranchAndPruneNodeLevel.h
//  GraphColoring
//
//  Created by Anton Logunov on 6/25/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __GraphColoring__BranchAndPruneNodeLevel__
#define __GraphColoring__BranchAndPruneNodeLevel__

#include <iostream>
#include <chrono>

#include "GC.hpp"


// BB stays for branch and bound
struct GC_BB_NodeLevel : GC {
    typedef chrono::seconds Duration;
    typedef chrono::system_clock Clock;
    typedef chrono::time_point<Clock> Time;
    
private:
    Duration duration;
    bool _verbose = false;
    
public:
    // duration in seconds!!!
    GC_BB_NodeLevel(const Duration& duration) : duration(duration) {}
    
    bool isVerbose() {
        return _verbose;
    }
    void setVerbose(bool b) {
        _verbose = b;
    }
    
    ColoredGraph solve(const Graph& gr) {
        ColoredGraph result(gr);
        ColoredGraph c_gr(gr);
        
        // rough solution
        GC_Naive naive;
        result = naive.solve(gr);
        
        Time startTime = Clock::now();
        
        stack<tuple<Node, Color>> st;
        stack<Node> nodeOrder;
        Node i;
        Color c;
        // iterative algorithm
        // choose next node, color it, repeat
        // also we keeping stack of our travelling
        // so we could bypass full solution space 
        while (true) {
            if (c_gr.uncoloredNodeCount() == 0) {
                result = c_gr;
                if (isVerbose()) cout << "sol: " << result.colorCount() << endl;
            }
            else if (c_gr.colorCount() < result.colorCount()){
                // want to find next node that will be colored next
                i = chooseNextUncoloredNode(c_gr);
                // that node probably will have many possible colors
                // so we will write it into our stack 
                vector<Color> colors = sortedNodePossibleColors(c_gr, i, result.colorCount()-1);
                if (colors.size() > 0) {  
                    nodeOrder.push(i);
                    for (Color c : colors) {
                        st.push({i, c});
                    }
                }
            }
            
            if (st.empty() || Clock::now() - startTime > duration) break;
            
            tie(i, c) = st.top();
            st.pop();
            
            // if we doing step back we should unset current color of followed node
            // possibly we will do multiple steps back in a row
            while (nodeOrder.top() != i) {
                c_gr.unsetColor(nodeOrder.top());
                nodeOrder.pop();
            }
            c_gr.setColor_2(i, c);
        }
        return result;
    }
    
    // taking node with maximum adjacent colors (pruning possible colors to try)
    // if have same amount of adjacent colors take one with greater uncolored node count
    Node chooseNextUncoloredNode(const ColoredGraph& c_gr) {
        auto *s = &c_gr.uncoloredNodes();
        return *max_element(s->begin(), s->end(), [&c_gr](Node i_1, Node i_2) {
            size_t n_1 = c_gr.adjacentColorCount(i_1),
                   n_2 = c_gr.adjacentColorCount(i_2);
            if (n_1 < n_2) return true;
            if (n_1 == n_2) {
                size_t u_1 = c_gr.adjacentUncoloredNodeCount(i_1),
                       u_2 = c_gr.adjacentUncoloredNodeCount(i_2);
                if (u_1 < u_2) return true;
            }
            return false;
        });
    }
    
    vector<Color> sortedNodePossibleColors(const ColoredGraph& c_gr, Node i, size_t maxColorCount) {
        set<Color> adjColors = c_gr.adjacentColors(i);
        set<Color> colors = c_gr.colors();
        // if maxColorCount not reached every time possible color may be new color
        if (maxColorCount > colors.size()) {
            colors.insert(colors.size() > 0 ? *colors.rbegin() + 1 : 0);  
        } 
        for (Color c : adjColors) {
            colors.erase(c);
        }
        vector<Color> result(colors.begin(), colors.end());
        return result;
    }
};


#endif /* defined(__GraphColoring__BranchAndPruneNodeLevel__) */
