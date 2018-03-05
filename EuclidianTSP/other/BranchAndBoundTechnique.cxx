////
////  BranchAndBoundTechnique.cpp
////  TravelingSalesman
////
////  Created by Anton Logunov on 7/6/13.
////  Copyright (c) 2013 Anton Logunov. All rights reserved.
////
//
//#include "BranchAndBoundTechnique.h"
//
//vector<int> naiveBB(const vector<Point>& ps, vector<int>& approxSolution, 
//                    double approxTotalDist, int kNearest, time_t time) {
//  int n = (int)ps.size();
//  vector<vector<int>> nearest(n);
//  for (int i = 0; i < n; i++) {
//    kNearestVertices(ps, ps[i], nearest[i], kNearest);
//  }
//  vector<bool> usedVertices(n, 0);
//  vector<tuple<int, int, int>> st; // 1-st int : cur index, 2-nd int : prev index, 3-rd int : cur index inside nearest
//  
//  vector<int> solution = approxSolution;
//  double minTotalDist = approxTotalDist;
//  
//  int iGlobal, iPrevGlobal, iLocal, nLocal, iNewGlobal;
//  double totalDist = 0;
//  st.push_back(make_tuple(0, -1, 0));
//  usedVertices[0] = 1;
//  int kLeft = n-1;
//  
//  time += std::time(NULL);
//  while (!st.empty() && std::time(NULL)-time < 0) {
//    tie(iGlobal, iPrevGlobal, iLocal) = st.back();
//    if (kLeft == 0) {
//      if (totalDist+ps[0].distance(ps[get<0>(st[n-1])]) < minTotalDist) {
//        minTotalDist = totalDist+ps[0].distance(ps[get<0>(st[n-1])]);
//        for (int i = 0; i < n; i++) {
//          solution[i] = get<0>(st[i]);
//        }
//        cout << "sol: " << minTotalDist << "\n";
//      }
//      ///
//      totalDist -= ps[iGlobal].distance(ps[iPrevGlobal]);
//      st.pop_back();
//      usedVertices[iGlobal] = 0;
//      kLeft++;
//      continue;
//    }
//    nLocal = (int)nearest[iGlobal].size();
//    while (iLocal < nLocal && usedVertices[nearest[iGlobal][iLocal]]) iLocal++;
//    if (iLocal == nLocal) {
//      ///
//      if (iGlobal != 0) totalDist -= ps[iGlobal].distance(ps[iPrevGlobal]);
//      st.pop_back();
//      usedVertices[iGlobal] = 0;
//      kLeft++;
//      continue;
//    }
//    get<2>(st.back()) = iLocal+1; 
//    iNewGlobal = nearest[iGlobal][iLocal];
//    totalDist += ps[iNewGlobal].distance(ps[iGlobal]);
//    st.push_back(make_tuple(iNewGlobal, iGlobal, 0));
//    usedVertices[iNewGlobal] = 1;
//    kLeft--;
//  }
//  return solution;
//}
//
