#pragma once

#include <iostream>
#include <unordered_set>

#include "ColoredGraph.hpp"

struct InfeasibleSwapper : ColoredGraph {
    time_t time;
    vector<int> feasibleColoring;
    set<int> uncoloredNodes;
    int nEliminatedColors;

    InfeasibleSwapper(
        const vector<vector<int>>& adjList, 
        const vector<int>& solution, 
        time_t time) 
    : ColoredGraph(adjList, solution), 
      feasibleColoring(solution), 
      nEliminatedColors(0), 
      time(time) 
    {
        for (int i = 0; i < nNodes; i++) {
            random_shuffle(adjList[i].begin(), adjList[i].end());
        }
    }

    void startTimer() {
        time += std::time(NULL);
    }
    bool isTimeOut() {
        return time - std::time(NULL) < 0;
    }

    // method name should be eliminateColor
    void prepareUncoloringNodes() {
        int c = nColors-nEliminatedColors-1;
        for (int i = 0; i < nNodes; i++) {
            if (coloring[i] == c) {
                unSetColor(i);
                uncoloredNodes.insert(i); 
            }
        }
        nEliminatedColors++;
    }

    vector<int> solve() {
        vector<int> add;
        vector<int> remove;

        startTimer();
        int numberOfSwaps = 0.1*nNodes;

        // while has time
        while (!isTimeOut()) {
            prepareUncoloringNodes();
            while (!isTimeOut() && uncoloredNodes.size() != 0) {
                add.clear();
                remove.clear();

                // what does it mean?

                for (int i : uncoloredNodes) {
                    // something like... was able to resolve or not
                    bool cont = true;
                    // can't reset color of unordered node

                    // need to keep a variable like currentColorCount or something
                    if (nAdjColors[i] >= nColors-nEliminatedColors-1) {
                        // try to find good neighbor for swap
                        for (int a : adjList[i]) {
                            if (coloring[a] != -1 &&
                                // only one such color

                                // prepare for kampe chain ???
                                adjColors[i][coloring[a]] == 1 &&
                                // will b able to change to another color
                                nAdjColors[a] < nColors-nEliminatedColors-1) 
                            {
                                setColor(i, coloring[a]);
                                remove.push_back(i);
                                // has to be added to uncolored?
                                unSetColor(a);
                                // now uncolored
                                i = a;
                                cont = false;
                                break;
                            }
                        }
                    } else {
                        // can change color without any hassle
                        remove.push_back(i);
                        cont = false;
                    } 
                  
                    if (cont) {
                        // random move
                        for (int a : adjList[i]) {
                            if (coloring[a] != -1 && adjColors[i][coloring[a]] == 1) {
                                setColor(i, coloring[a]);
                                remove.push_back(i);
                                unSetColor(a);
                                add.push_back(a);
                                break;
                            }
                        }
                        continue;
                    }
                  
                    for (int c = 0; c < nColors-nEliminatedColors; c++) {
                        if (adjColors[i][c] == 0) {
                            setColor(i, c);
                            break;
                        }
                    }
                }

                // remove and add to uncolored
                for (int r : remove) uncoloredNodes.erase(r);
                for (int a : add) uncoloredNodes.insert(a);

                for (int k = 0; k < numberOfSwaps; k++) {
                    int c, i;
                    for (int j : uncoloredNodes) {
                        //while (coloring[i = rand()%nNodes] == -1);

                        // take random adj of j... make sure it's colored
                        // can it be that they all are uncolored???? can it be??? or it has only one adj and it's uncolored
                        while (coloring[i = adjList[j][rand()%adjList[j].size()]] == -1);

                        // find any adj color of i.. also can run in endless loop.
                        while (adjColors[i][c = rand()%(nColors-nEliminatedColors)] == 0);

                        // run kempe chains
                        makeKempeChain(i, c);
                    }
                }
            }

            // save new solution
            if (uncoloredNodes.size() == 0) { 
                feasibleColoring = coloring;
                cout << "sol: " << nColors-nEliminatedColors << "\n";
            }
        }
        return feasibleColoring;
    }  
};