//
//  InfeasibleSwapper.h
//  GraphColoring
//
//  Created by Anton Logunov on 6/26/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __GraphColoring__InfeasibleSwapper__
#define __GraphColoring__InfeasibleSwapper__

#include <iostream>
#include <unordered_set>

#include "ColoredGraph.hpp"
/*
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
        while (!isTimeOut()) {
            prepareUncoloringNodes();
            while (!isTimeOut() && uncoloredNodes.size() != 0) {
                add.clear();
                remove.clear();
                
                bool cont;
                for (int i : uncoloredNodes) {
                    cont = true;
                    // can't reset color of unordered node
                    if (nAdjColors[i] >= nColors-nEliminatedColors-1) {
                        // try to find good neighbor for swap
                        for (int a : adjList[i]) {
                            if (coloring[a] != -1 && 
                                adjColors[i][coloring[a]] == 1 && 
                                nAdjColors[a] < nColors-nEliminatedColors-1) 
                            {
                                setColor(i, coloring[a]);
                                remove.push_back(i);
                                unSetColor(a);
                                i = a;
                                cont = false;
                                break;
                            }
                        }
                    } else {
                        remove.push_back(i);
                        cont = false;
                    } 
                  
                    if (cont) {
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
                for (int r : remove) uncoloredNodes.erase(r);
                for (int a : add) uncoloredNodes.insert(a);

                set<int> set0, set1;
                for (int k = 0; k < numberOfSwaps; k++) {
                    int c, i;
                    for (int j : uncoloredNodes) {
                        //while (coloring[i = rand()%nNodes] == -1);
                        while (coloring[i = adjList[j][rand()%adjList[j].size()]] == -1);
                        while (adjColors[i][c = rand()%(nColors-nEliminatedColors)] == 0);
                        set0.clear();
                        set1.clear();
                        makeKempeChain(i, c, set0, set1);
                        swapColors(set0, set1);
                    }
                }
            }
            if (uncoloredNodes.size() == 0) { 
                feasibleColoring = coloring;
                cout << "sol: " << nColors-nEliminatedColors << "\n";
            }
        }
        return feasibleColoring;
    }  
};
*/
#endif /* defined(__GraphColoring__InfeasibleSwapper__) */
