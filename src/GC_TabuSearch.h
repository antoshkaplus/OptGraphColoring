//
//  GC_TabuSearch.h
//  GraphColoring
//
//  Created by Anton Logunov on 3/28/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __GraphColoring__GC_TabuSearch__
#define __GraphColoring__GC_TabuSearch__

#include <iostream>
#include <random>


#include "GC.h"


struct GC_TabuSearch : GC {
private:
    typedef uniform_int_distribution<Node> NodeDistribution;
    typedef pair<Node, Color> TabuKey;
    typedef size_t Index;
    
    struct TabuList : map<TabuKey, Index> {
        
        Index currentIteration;
        size_t tenure;
        
        bool isTabu(const TabuKey& key) {
            auto it = find(key);
            if (it != end()) {
                if (it->second >= currentIteration) return true;
                erase(it);
            }
            return false;
        }
        
        void insert(const TabuKey& key) {
            (*this)[key] = currentIteration + tenure;
        }
    };
    
    
    default_random_engine rng;
    TabuList tabuList;
    Index currentIteration;
    ColoredGraph *c_gr;
    // uncolored nodes tatal degree that was achieved so far
    size_t minUncoloredNodesTotalDegree;
    bool _verbose = false;
    
public:
    ColoredGraph solve(const Graph& gr);

    bool isVerbose() {
        return _verbose;
    }
    void setVerbose(bool b) {
        _verbose = b;
    }

    Node randomUncoloredNode() {
        set<Node> s = c_gr->uncoloredNodes();
        NodeDistribution d(0, (Node)s.size()-1);
        return vector<Node>(s.begin(), s.end())[d(rng)];
    }

    bool reduceUncoloredNodeCount() {
        ColoredGraph& c_gr = *this->c_gr;
        bool didReduce = false;
        const set<Node> &s = c_gr.uncoloredNodes();
        for (Node i : vector<Node>(s.begin(), s.end())) {
            if (c_gr.adjacentColorCount(i) < c_gr.colorCount()) {
                c_gr.setColor(i);
                didReduce = true;
            }
        }
        return didReduce;
    }

    Color colorWithMinTotalDegree() {
        CountMap<Color> cm;
        ColoredGraph& c_gr = *this->c_gr;
        for (Node i = 0; i < c_gr.nodeCount; i++) {
            cm.increase(c_gr.color(i), c_gr.adjacencyList[i].size());
        }
        pair<Color, size_t> min = *cm.begin();
        for (auto& p : cm) {
            if (p.second < min.second) {
                min = p;
            }
        }
        return min.first;
    }

    Degree uncoloredNodesTotalDegree() {
        Degree d = 0;
        for (Node i : c_gr->uncoloredNodes()) {
            d += c_gr->degree(i);
        }
        return d;
    }

    Degree colorTotalDegree(Color c) {
        ColoredGraph& c_gr = *this->c_gr;
        Degree d = 0;
        for (Node i = 0; i < c_gr.nodeCount; i++) {
            if (c_gr.color(i) == c) d += c_gr.adjacencyList[i].size();
        }
        return d;
    }

    // returns whether did improvement this time
    bool localSearch(Node i) {
        ColoredGraph &c_gr = *this->c_gr;
        tabuList.currentIteration = currentIteration;

        Color minColor;
        vector<Node> adjNodes, minAdjNodes;
        size_t totalDegree, minTotalDegree;
        bool isMinInitialized = false;

        // want exchange unset node with adjacent colored nodes
        // make one node colored and adjacent with same color become uncolored 
        for (Color c : c_gr.adjacentColors(i)) {
            adjNodes.clear();
            totalDegree = 0;
            for (Node j : c_gr.adjacencyList[i]) {
                if (c_gr.color(j) == c) {
                    adjNodes.push_back(j);
                    totalDegree += c_gr.degree(j);
                }
            }
            if (!isMinInitialized || totalDegree < minTotalDegree) {
                // if tabu then we use it only if it a lot better
                if (tabuList.isTabu(TabuKey(i, c)) && 
                    uncoloredNodesTotalDegree() - c_gr.degree(i) + totalDegree 
                    >= minUncoloredNodesTotalDegree) {
                    
                    continue;
                }
                minTotalDegree = totalDegree;
                minAdjNodes = adjNodes;
                minColor = c;
                isMinInitialized = true;
            }
        }

        if (isMinInitialized) {
            c_gr.setColor(i, minColor);
            tabuList.insert(TabuKey(i, minColor));
            for (Node j : minAdjNodes) {
                c_gr.unsetColor(j);
            } 
            return true;
        }
        return false;
    }
};




#endif /* defined(__GraphColoring__GC_TabuSearch__) */
