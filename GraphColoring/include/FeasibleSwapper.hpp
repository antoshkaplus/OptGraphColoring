#pragma once

#include <iostream>

#include "ColoredGraph.hpp"

// get in feasible solution and tries to improve
struct FeasibleSwapper : ColoredGraph {


    time_t time;

    void startTimer() {
        time += std::time(NULL);
    }

    bool isTimeOut() {
        return time - std::time(NULL) < 0;
    }

    vector<int> solve() {
        startTimer();
        int numberOfSwaps = 0.1 * nNodes;
        while (!isTimeOut()) {

            bool canRepeat;
            vector<pair<int, int>> r;
            while (true) {
                canRepeat = false;
                // try to crowd up nodes with the same color
                // leaving some colors with little nodes
                for (int i = 0; i < nNodes; i++) {
                    r.clear();
                    possibleColorsWithMoreOrEqualNodes(i, r);
                    if (r.size()) {
                        // take element with most nodes

                        // color - how many of that color ??
                        pair<int, int> p = *max_element(r.begin(), r.end(),
                                                        [](pair<int, int>& p1, pair<int, int>& p2) {
                                                            return p1.second < p2.second;
                                                        });
                        // set this color to current node
                        if (p.second > 0) {
                            setDifferentColor(i, p.first);
                            canRepeat = true;
                        }
                    }
                }
                if (!canRepeat) break;
            }

            set<int> set0, set1;
            int n0, n1, v;
            int k = 0;
            // now use kempe chain
            while (k != numberOfSwaps && !isTimeOut()) {
                // pick random node
                int c, i = rand() % nNodes;
                // find colored node
                while (adjColors[i][c = rand() % nColors] == 0);

                // start kempe chain
                set0.clear();
                set1.clear();
                makeKempeChain(i, c, set0, set1);
                // set0.size() > set1.size() and colors0 <= colors1
                // set0.size() < set1.size() and colors0 >= colors1
                n0 = (int) set0.size();
                n1 = (int) set1.size();
                v = compareColorsByNumberOfNodes(coloring[i], c);
                // if equal number of nodes - fine. if more of n0 and more c colored nodes. if more of n1 and more i colored nodes
                // then we do the swap
                if (n0 == n1 || (n0 > n1 && v <= 0) || (n0 < n1 && v >= 0)) {
                    swapColors(set0, set1);
                    k++;
                }
            }
        }

        // call reduce colors to possible have less colors
        return reduceColors(coloring);
    }

    FeasibleSwapper(const vector<vector<int>>& adjList, const vector<int>& solution, time_t time)
        : ColoredGraph(adjList, solution), time(time) {}
};




