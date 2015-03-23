//
//  FeasibleSwapper.h
//  GraphColoring
//
//  Created by Anton Logunov on 6/25/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __GraphColoring__FeasibleSwapper__
#define __GraphColoring__FeasibleSwapper__

#include <iostream>

#include "ColoredGraph.hpp"
/*
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
    int numberOfSwaps = 0.1*nNodes;
    while (!isTimeOut()) {
    
      bool canRepeat;
      vector<pair<int, int>> r;
      while (true) {
        canRepeat = false;
        for (int i = 0; i < nNodes; i++) {
          r.clear();
          possibleColorsWithMoreOrEqualNodes(i, r);
          if (r.size()) {
            pair<int, int> p = *max_element(r.begin(), r.end(), 
                                            [](pair<int, int>& p1, pair<int, int>& p2)
                                            { return p1.second < p2.second; });
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
      while (k != numberOfSwaps && !isTimeOut()) {
        int c, i = rand()%nNodes;
        while (adjColors[i][c = rand()%nColors] == 0);
        set0.clear();
        set1.clear();
        makeKempeChain(i, c, set0, set1);
        // set0.size() > set1.size() and colors0 <= colors1
        // set0.size() < set1.size() and colors0 >= colors1
        n0 = (int)set0.size();
        n1 = (int)set1.size();
        v = compareColorsByNumberOfNodes(coloring[i], c);
        if (n0 == n1 || (n0 > n1 && v <= 0) || (n0 < n1 && v >= 0)) {
          swapColors(set0, set1);
          k++;
        }
      }
    }
    return reduceColors(coloring);
  }  

  FeasibleSwapper(const vector<vector<int>>& adjList, const vector<int>& solution, time_t time) 
      : ColoredGraph(adjList, solution), time(time) {}
};
*/
#endif /* defined(__GraphColoring__FeasibleSwapper__) */




