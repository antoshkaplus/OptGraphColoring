////
////  BranchAndBoundTechnique.h
////  TravelingSalesman
////
////  Created by Anton Logunov on 7/6/13.
////  Copyright (c) 2013 Anton Logunov. All rights reserved.
////
//
//#ifndef __TravelingSalesman__BranchAndBoundTechnique__
//#define __TravelingSalesman__BranchAndBoundTechnique__
//
//#include <iostream>
//
////#include "support.h"
///*
//vector<int> naiveBB(const vector<Point>& ps, vector<int>& approxSolution, 
//                    double approxTotalDist, int kNearest, time_t time);
//
//// i will modify this thing for nNearest possibility
//struct EdgeSelectionBB {
//
//  int nCities;
//  double cost;
//  double bestTotalScore;
//  int nIncludedEdges;
//  int nEdges;
//  vector<Edge> curSolution;
//  vector<Edge> bestSolution;
//  vector<Edge> edges;
//  
//  vector<array<int, 2>> lowerBound; // index in nearestPoints
//  vector<vector<int>> nearestPoints;
//  vector<vector<int>> inverseNearestPoints;
//  vector<vector<double>> distances;
//  vector<vector<bool>> isEdgeInUse;
//  vector<int> pointDegree;
//  vector<int> nPossibleEdges;
//  
//  
//  
//  EdgeSelectionBB(const vector<Point>& ps, vector<int>& approxSolution, 
//                  double approxTotalDist, time_t time) {
//    nCities = (int)ps.size();
//    cost = 0.;
//    bestTotalScore = approxTotalDist;
//    nIncludedEdges = 0;
//    int a_prev = approxSolution.back();
//    for (int a : approxSolution) {
//      bestSolution.push_back(Edge(a_prev, a));
//      a_prev = a;
//    }
//    distances.resize(nCities, vector<double>(nCities));
//    for (int i = 0; i < nCities; i++) {
//      distances[i][i] = MAXFLOAT;
//      for (int j = i+1; j < nCities; j++) {
//        edges.push_back(Edge(i, j));
//        distances[i][j] = distances[j][i] = ps[i].distance(ps[j]);
//      }
//    }
//    nEdges = (int)edges.size();
//    nearestPoints.resize(nCities);
//    lowerBound.resize(nCities);
//    pointDegree.resize(nCities);
//    nPossibleEdges.resize(nCities);
//    for (int i = 0; i < nCities; i++) {
//      kNearestVertices(ps, ps[i], nearestPoints[i], nCities-1);
//      lowerBound[i] = {0, 1};
//      cost += (distances[i][nearestPoints[i][lowerBound[i][0]]] + 
//               distances[i][nearestPoints[i][lowerBound[i][1]]])/2.;
//      pointDegree[i] = 0;
//      nPossibleEdges[i] = nCities-1;
//    }
//    isEdgeInUse.resize(nCities, vector<bool>(nCities, false));
//    
//    vector<vector<double>>& d = distances;
//    sort(edges.begin(), edges.end(), 
//         [&d](const Edge& e1, const Edge& e2) { return d[e1.first][e1.second] < d[e2.first][e2.second]; });
//  }
//  
//  enum Action {
//    INCLUDE,
//    LEAVE,
//    OPEN
//  };
//  
//  void log() {
//    if (true) {
//      cout << "lowerbound: \n";
//      for (int i = 0; i < nCities; i++) {
//        cout << i << " : ";
//        for (int q = 0; q < 2-pointDegree[i]; q++) {
//          cout << nearestPoints[i][lowerBound[i][q]] << " ";
//        }
//        cout << "\n";
//      }
//    }
//    
//    if (true) {
//      cout << "number of possible edges: \n";
//      for (int i = 0; i < nCities; i++) {
//        cout << i << ":" << nPossibleEdges[i] << " ";
//      }
//      cout << "\n\n";
//    }
//  }
//  
//  int findOtherEnd(int i1) {
//    int a = i1, a_prev;
//    int j1, j2;
//    bool found;
//    do {
//      found = false;
//      for (Edge& e : curSolution) {
//        j1 = e.first;
//        j2 = e.second;
//        if (j1 == a && j2 != a_prev) {
//          a = j2;
//          a_prev = j1;
//          found = true;
//          break;
//        }
//        if (j2 == a && j1 != a_prev) {
//          a = j1;
//          a_prev = j2;
//          found = true;
//          break;
//        }
//      }
//    } while (found);
//    return a;
//  }
//  
//  // index of the edge; action 0:open edge, 1:include edge, 2:leave edge 
//  typedef tuple<int, int> Element;
//  vector<int> solve() {
//  
//    vector<Element> st;
//    st.push_back(Element(0, LEAVE));
//    st.push_back(Element(0, INCLUDE));
//    int i, a;
//    bool bCanLeave, bCanInclude;
//    double leaveCost, includeCost;
//    while (!st.empty()) {
//      tie(i,a) = st.back();
//      if (a == OPEN) {
//        openEdge(i);
//        st.pop_back();
//        continue;
//      }
//       
//      get<1>(st.back()) = OPEN;
//      if (a == INCLUDE) {
//        includeEdge(i);
//      } else { // LEAVE
//        leaveEdge(i);
//      }
//      
//      if (cost >= bestTotalScore) continue;
//      
//      if (nIncludedEdges == nCities) {
//        bestSolution = curSolution;
//        bestTotalScore = cost;
//        continue;
//      }
//      
//      if (i+1 == nEdges) continue;
//      
//      bCanLeave = canLeaveEdge(i+1);
//      bCanInclude = canIncludeEdge(i+1);
//      if (bCanInclude && bCanLeave) {
//        includeEdge(i+1);
//        includeCost = cost;
//        openEdge(i+1);
//        
//        leaveEdge(i+1);
//        leaveCost = cost;
//        openEdge(i+1);
//        
//        if (includeCost > leaveCost) { // leave is better
//          st.push_back(Element(i+1, INCLUDE));
//          st.push_back(Element(i+1, LEAVE));
//        }
//        else { // include is better
//          st.push_back(Element(i+1, LEAVE));
//          st.push_back(Element(i+1, INCLUDE));
//        }
//      } 
//      else if (bCanLeave) st.push_back(Element(i+1, LEAVE));
//      else if (bCanInclude) st.push_back(Element(i+1, INCLUDE));
//    }
//    
//    return edgesToSolution(bestSolution);
//  }
//  
//  bool canIncludeEdge(int i) {
//    /* can include edge when
//     * 1) both points have degree of 1 and it's a last edge needed to include
//     * 2) else one of the points or both should have degree of 0 so that subtour didn't close
//     * 3) degree of 2 is not allowed
//     * 4) if after connection there will be point of degree 2 anywhere, i can get number of  
//     *    possible edges that is incompatible with point degree for some point 
//     */
//    vector<int>& pd = pointDegree; 
//    Edge& e = edges[i];
//    int i1 = e.first, 
//        i2 = e.second; 
//        
//    // rules : 1, 2, 3
//    if (!(((pd[i1] != 2 && pd[i2] != 2) && (pd[i1] == 0 || pd[i2] == 0)) 
//          || (pd[i1] == 1 && pd[i2] == 1 && (nIncludedEdges == nCities-1 || findOtherEnd(i1) != i2)))) return false; 
//    
//    int k;
//    bool b1 = pd[i1] == 1,
//         b2 = pd[i2] == 1;
//    for (int i = 0; i < nCities; i++) {
//      if (i != i1 && i != i2) {
//        k = 0;
//        if (!isEdgeInUse[i1][i]) k += b1;
//        if (!isEdgeInUse[i2][i]) k += b2;
//        if (nPossibleEdges[i]-k < 2-pd[i]) return false;
//      } 
//    }
//    
//    return true;
//  }
//  
//  bool canLeaveEdge(int i) {
//    /* can leave edge when there are enough possible edges besides for both points
//     * if there is point with degree 2 than I already count out this edge
//     */
//    Edge& e = edges[i]; 
//    int i1 = e.first,
//        i2 = e.second;
//    return pointDegree[e.first] == 2 || pointDegree[e.second] == 2 || 
//          (nPossibleEdges[i1] > 2-pointDegree[i1] && nPossibleEdges[i2] > 2-pointDegree[i2]);
//  }
//  
//  
//  int lowerBoundFindReplacement(int i) {
//    /* should be sure that replacement exists */
//    int k = pointDegree[i] == 1 ? lowerBound[i][0] : lowerBound[i][1];
//    vector<int>& np = nearestPoints[i];
//    while (pointDegree[np[++k]] == 2 || isEdgeInUse[i][np[k]]);
//    return k;
//  }
//  
//  
//  void lowerBoundLostRelation(int i1, int i2) {
//    // i1 => i2
//    array<int, 2>& lb = lowerBound[i1];
//    vector<int>& np = nearestPoints[i1];
//    vector<double>& d = distances[i1];
//    int k, pd = pointDegree[i1];
//    for (int q = 0; q < 2-pd; q++) {
//      if (np[lb[q]] == i2) {
//        k = lowerBoundFindReplacement(i1);
//        cost -= d[i2]/2.;
//        cost += d[np[lb[q] = k]]/2.;
//        break;
//      }
//    }
//    if (pd == 0 && d[np[lb[0]]] > d[np[lb[1]]]) swap(lb[0], lb[1]);
//  }
//  
//  
//  void lowerBoundSuggestRelation(int i1, int i2) {
//    array<int, 2>& lb = lowerBound[i1];
//    vector<int>& np = nearestPoints[i1];
//    vector<double>& d = distances[i1];
//    
//    int k = -1;
//    while (np[++k] != i2); /* if I could have inverseNearestPointss */
//    double c = distances[i1][i2]/2.;
//  
//    if (pointDegree[i1] == 0) {
//      if (lb[0] > k) {
//        cost -= d[np[lb[1]]]/2.;
//        swap(lb[0], lb[1]);
//        lb[0] = k;
//        cost += c;
//      }
//      else if (lb[0] != k && lb[1] > k) {
//        cost -= d[np[lb[1]]]/2.;
//        lb[1] = k;
//        cost += c;
//      }
//    } 
//    else if (pointDegree[i1] == 1) { 
//      if (lb[0] > k) {
//        cost -= d[np[lb[0]]]/2.;
//        lb[0] = k;
//        cost += c;
//      }
//    } 
//  }
//  
//  // called inside include edge 
//  void lowerBoundReduce(int i) {
//    array<int, 2>& lb = lowerBound[i];
//    vector<int>& np = nearestPoints[i];
//    vector<double>& d = distances[i];
//    
//    if (pointDegree[i] == 0) {
//      // that edge can no longer be free
//      if (isEdgeInUse[i][np[lb[0]]]) {
//        swap(lb[0], lb[1]);
//      }
//      cost -= d[np[lb[1]]]/2.;
//    }
//    else {
//      cost -= d[np[lb[0]]]/2.;
//    }
//    pointDegree[i]++;
//  }
//  
//  void lowerBoundRaise(int i) {
//    int k, b;
//    if (pointDegree[i] == 2) {
//      // start with 0-th nearestPoint
//      k = 0;
//      // where will add
//      b = 0;
//    } else {
//      // start with next to the first lower bound
//      k = lowerBound[i][0]+1;
//      b = 1;
//    }
//    
//    vector<int>& np = nearestPoints[i];
//    while (pointDegree[np[k]] == 2 || isEdgeInUse[i][np[k]]) k++;
//    lowerBound[i][b] = k;
//    cost += distances[i][np[k]]/2.;
//    
//    pointDegree[i]--;
//  }
//
//  void leaveEdge(int i) {
//    int i1 = edges[i].first,
//        i2 = edges[i].second;
//    isEdgeInUse[i1][i2] = isEdgeInUse[i2][i1] = true;
//    
//    /* if opposite point has degree 2 that edge should be already counted out for the first point */
//    if (pointDegree[i1] != 2) nPossibleEdges[i2]--;
//    if (pointDegree[i2] != 2) nPossibleEdges[i1]--; 
//    
//    if (pointDegree[i1] == 2 || pointDegree[i2] == 2) {
//      lowerBoundLostRelation(i1, i2);
//      lowerBoundLostRelation(i2, i1);
//    }
//  }
//
//  void includeEdge(int i) {
//    curSolution.push_back(edges[i]);
//    nIncludedEdges++;
//    Edge& e = edges[i];
//    int i1 = e.first, 
//        i2 = e.second;
//    isEdgeInUse[i1][i2] = isEdgeInUse[i2][i1] = true;
//    cost += distances[i1][i2];
//    nPossibleEdges[i1]--;
//    nPossibleEdges[i2]--;
//    lowerBoundReduce(i1);
//    lowerBoundReduce(i2);
//    // should observe all
//    for (int iS : {i1, i2}) {
//      if (pointDegree[iS] == 2) {
//        for (int i = 0; i < nCities; i++) {
//          if (i != i1 && i != i2 && !isEdgeInUse[i][iS]) {
//            nPossibleEdges[i]--;
//            lowerBoundLostRelation(i, iS);
//          } 
//        }
//      }
//    }
//  }
//
//  void openEdge(int i) {
//    Edge& e = edges[i];
//    int i1 = e.first,
//        i2 = e.second;
//    isEdgeInUse[i1][i2] = isEdgeInUse[i2][i1] = false;
//    
//    if (curSolution.size() && curSolution.back() == edges[i]) {
//      // always
//      nPossibleEdges[i1]++;
//      nPossibleEdges[i2]++;
//      
//      curSolution.pop_back();
//      nIncludedEdges--;
//      cost -= distances[i1][i2];
//      
//      // if there is one pointDegree somewhere i can change lowerBound value
//      lowerBoundSuggestRelation(i1, i2);
//      lowerBoundSuggestRelation(i2, i1);
//      
//      pointDegree[i2]--;
//      lowerBoundRaise(i1);
//      pointDegree[i2]++;
//      lowerBoundRaise(i2);
//      for (int iS : {i1, i2}) {
//        if (pointDegree[iS] == 1) {
//          for (int i = 0; i < nCities; i++) {
//            if (i != i1 && i != i2 && !isEdgeInUse[i][iS]) {
//              nPossibleEdges[i]++;
//              lowerBoundSuggestRelation(i, iS);
//            }
//          }
//        }
//      }
//    } 
//    else { 
//      // edge was left
//      if (pointDegree[i1] != 2) {
//        nPossibleEdges[i2]++;
//        lowerBoundSuggestRelation(i2, i1);
//      } 
//      if (pointDegree[i2] != 2) {
//        nPossibleEdges[i1]++;
//        lowerBoundSuggestRelation(i1, i2);
//      }
//    }
//  }
//  
//};
//*/
//
//#endif /* defined(__TravelingSalesman__BranchAndBoundTechnique__) */
