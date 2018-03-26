#pragma once

#include <iostream>
#include <random>


#include "GC.hpp"


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
    Index maxIteration = 100000;

public:
    ColoredGraph solve(const Graph& gr) override {
        // how many iterations can't swap colors
        // trying out value of 10
        tabuList.tenure = 10;

        ColoredGraph result(gr);
        GC_Naive_2 naive;
        ColoredGraph c_gr(naive.solve(gr));
        this->c_gr = &c_gr;

        while (c_gr.uncoloredNodeCount() == 0) {
            result = c_gr;
            // going from complete k+1 to partial k colored graph
            Color eliminatingColor = colorWithMinTotalDegree();
            for (Node i = 0; i < c_gr.nodeCount(); i++) {
                if (c_gr.color(i) == eliminatingColor) c_gr.unsetColor(i);
            }
            tabuList.clear();
            currentIteration = 0;
            minUncoloredNodesTotalDegree = uncoloredNodesTotalDegree();
            while (c_gr.uncoloredNodeCount() > 0 &&
                   currentIteration < maxIteration) {
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

    string name() override {
        return "TabuSearch";
    }

    void setMaxIteration(Count iteration) {
        maxIteration = iteration;
    }

    bool isVerbose() {
        return _verbose;
    }
    void setVerbose(bool b) {
        _verbose = b;
    }

    Node randomUncoloredNode() {
        NodeDistribution d(0, (Node)c_gr->uncoloredNodeCount()-1);
        return c_gr->uncoloredNodes() [d(rng)];
    }

    bool reduceUncoloredNodeCount() {
        ColoredGraph& c_gr = *this->c_gr;
        bool didReduce = false;
        auto uncolored = c_gr.uncoloredNodes();
        for (Node i : uncolored) {
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
        for (Node i = 0; i < c_gr.nodeCount(); i++) {
            cm.increase(c_gr.color(i), c_gr.degree(i));
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
        for (Node i = 0; i < c_gr.nodeCount(); i++) {
            if (c_gr.color(i) == c) d += c_gr.degree(i);
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
            for (Node j : c_gr.nextNodes(i)) {
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