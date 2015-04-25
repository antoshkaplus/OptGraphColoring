



nearestPoints


// first point should start with point degree - 1, and i don't need close path


struct infSol {
  
  vector<int[2]> lowerBound;
  vector<vector<int>> inverseLowerBound; // maybe use list?
  vector<vector<int>>& nearestPoints; // can't take only K of them
  vector<int> curNearestIndex; // next index where to search new nearest point
  int maxNearestIndex;
  vector<int> solution
  vector<vector<double>>& distances;
  int nCities;
  double cost;
  
  infSol() {
    cost = 0.;
    lowerBound.resize(nCities);
    inverseLowerBound.resize(nCities);
    curNearestIndex.resize(nCities);
    for (int i = 0; i < nCities; i++) {
      lowerBound[i][0] = 0;
      lowerBound[i][1] = 1;
      curNearestIndex[i] = 2;
      inverseLowerBound[nearestPoints[0]].push_back(i);
      inverseLowerBound[nearestPoints[1]].push_back(i);
      cost += distances[i][nearestPoints[0]]/2.;
      cost += distances[i][nearestPoints[1]]/2.;
    }
  }
  
  void append(int i2) {
    i1 = solution.back();
    solution.push_back(i2);
    usedPoints[i1] = true;
    
    cost -= distances[i1][nearestPoints[lowerBound[i1][0]]]/2.;
    cost -= distances[i2][nearestPoints[lowerBound[i2][1]]]/2.;
    
    cost += distances[i1][i2];
    
    // now i1 is fully closed
    for (int i3 : inverseNearestPoints[i1]) { // edges where i1 plays big role
      if (!usedPoint[i3] && i3 != i2) {
        if (lowerBound[i3][0] == i1) {
          swap(lowerBound[i3][0], lowerBound[i3][1]);
        }
        cost -= distances[i3][nearestPoints[i3][lowerBound[i3][1]]]/2.;
        int cur = curNearestIndex[i3];
        for (; cur < maxNearestIndex; cur++) {
          if (!usedPoints[nearestPoints[cur]]) {
            lowerBound[i3][1] = cur;
            inverseNearestPoints[cur].push_back(i3);
            cost += distances[i3][cur]/2.;
            break;
          }
        }
        curNearestIndex[i3] = cur+1;
      }
    }
    inverseNearestPoints[i1].clear();
  }
  
  void pop_last() {
    i2 = solution.back();
    solution.pop_back();
    i1 = solution.back();
    usedPoints[i1] = false;
    
    cost -= distances[i1][i2];
    
    cost += distances[i1][nearestPoints[lowerBound[i1][0]]]/2.;
    cost += distances[i2][nearestPoints[lowerBound[i2][1]]]/2.;
    
    // put indices inside inverseNearestPoints
    
    for (int i = 0; i < nCities; i++) {
      if (!usedPoints[i] && i != i1 && i != i2) {
        // can put this i1 point inside that i edges
        // don't forget about inverseNearestPoints
        i, i1
        if ()
      
      }
      
    }
  }
  
  
  
}

/* I should get lower bound for one point or give up lower bound for one point
   then after appending point i kill full point and then
   when popping point i don't need to find new distances for her, because i back to some particular state.
   
   i only find new distances when point is free... all the time

*/

/* begin of another implementation */

void append(i2) {
  i1 = solution.back();
  solution.push_back(i2);
  
  usedPoints[i1] = true;
  cost += distances[i1][i2];
  cost -= lowerBound[i1][0].cost;
  swap(lowerBound[i2][0], lowerBound[i2][1]);
  cost -= lowerBound[i2][0].cost;
  
  for (int i = 0; i < nCities; i++) {
    if (!usedPoints[i] && i != i2) {
      LB& lb = lowerBound[i];
      if (nearestPoints[i][lb[0].index] == i1) {
         swap(lb[0], lb[1]);
      }
      if (nearestPoints[i][lb[1].index] == i1) {
        cost -= lb[1].cost;
        for (int cur = curNearestIndex[i]; cur < maxNearestIndex; cur++) {
          if (!usedPoints[nearestPoints[cur]]) {
            lb[1].index = cur;
            cost += (lb[1].cost = distances[i][nearestPoints[cur]]/2.);
            curNearestIndex[i3] = cur+1;
            break;
          }
        }
      }
    }
  }
}


void pop_last() {
    i2 = solution.back();
    solution.pop_back();
    i1 = solution.back();
    usedPoints[i1] = false;
    
    cost -= distances[i1][i2];
    
    //cost += lowerBound[i1][0].cost;
    //cost += distances[i2][nearestPoints[lowerBound[i2][1]]]/2.;
    
    // put indices inside inverseNearestPoints
    
  
  
  
}


/* end of another implementation */




void go(i_prev) {
  // i_prev has pointDegree[i_prev] = 1
  for (int a : nearestPoints[i_prev]) { // used vs not used
    if (pointDegree[a] < 2) {
      // do i really need to romp with 1 degree vertex ???
      infSol.append(a);
      kLeft--;
      if (infSol.cost < bestCost) {
        if (kLeft == 0) {
          bestCost = infSol.cost
          // get new solution
        }
        else {
          go(a);
        }
      }
      kLeft++;
      infSol.pop_last();
    }
  }
}
 







edges = [];

st.push(0);



while (!st.empty()) {
  i = st.back();
  e = edges[i];
  i1 = e.first;
  i2 = e.second;
  while (i < nEdges &&
         ((pointDegree[i1] == 1 && pointDegree[i2] == 1) ||
          pointDegree[i1] == 2 ||
          pointDegree[i2] == 2)) i++;
  if (i == nEdge) {
    st.pop();
    continue;
  }
  if ()
  
  
}




isOpenEdges = [bool] // vector 

bool takeEdge(i) {
  sol.push(i);
  cost += edges[i].distance;
  i1, i2 = edges[i]
  for (i1, i2) :
    if pointDegree[i] == 0:
      lowerBound[i] delete one thing
      reassign cost
  
  if (pointDegree[i1] == 2 || pointDegree[i2] == 2) {
    k = pointDegree[i1] == 2 ? i1 : i2;
    for (int i = 0; i < nCities; i++) {
      if (pointDegree[i] < 2) {
        if (lowerBound[i] has k) {
          reset lowerBound[i]
          reassign cost
        }
      }
    }
  }
}

bool leaveEdge(i) {
  "can't choose that edge for lower bound for i1 and i2"
}

void openEdge(i) {
   "try to assign this edge to lowerbound i1 or i2"
}

"should i use inverseLowerBound"



