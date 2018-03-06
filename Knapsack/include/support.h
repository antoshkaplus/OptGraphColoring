//
//  support.h
//  Knapsack
//
//  Created by Anton Logunov on 6/20/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef Knapsack_support_h
#define Knapsack_support_h


#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct Item {
  int w;
  int v;
};

pair<int, vector<int>> usual(const vector<Item>& items, int capacity) {
  vector<int> res(items.size());
  int n = (int)items.size();
  int k = capacity;
  vector<int> buffer((k+1)*(n+1));
  // column 0 : 0 items
  for (int i = 0; i < k+1; i++) {
    buffer[i*(n+1)] = 0;
  }
  // row 0 : 0 capacity
  for (int i = 0; i < n+1; i++) {
    buffer[i] = 0;
  }
  // loop by items
  for (int i = 1; i < n+1; i++) {
    const Item& t = items[i];
    // loop by capacity
    for (int j = 0; j < min(t.w, k+1); j++) {
      buffer[j*(n+1) + i] = buffer[j*(n+1) + i-1];
    }
    for (int j = t.w; j < k+1; j++) {
      buffer[j*(n+1) + i] = max(buffer[j*(n+1) + i-1], buffer[(j-t.w)*(n+1)+i-1] + t.v);
    }
  }
  
  int w = k;
  for (int i = n; i > 0; i--) {
    if (buffer[w*(n+1) + i] == buffer[w*(n+1) + i-1]) {
      res[i] = 0;
    }
    else {
      res[i] = 1;
      w -= items[i].w;
    }
  }
  
  return pair<int, vector<int>>(buffer[(k+1)*(n+1)-1], res);
}

pair<int, vector<int>> greedy(const vector<Item>& items, int capacity) {
  vector<int> res(items.size());
  int n = (int)items.size();
  vector<double> keys(n);
  for (int i = 0; i < n; i++) {
    keys[i] = 1.*items[i].v/items[i].w;
  }
  vector<int> indices(n);
  for (int i = 0; i < n; i++) indices[i] = i;
  std::sort(indices.begin(), indices.end(), [&keys](int i1, int i2){ return keys[i1] > keys[i2]; });
  
  int v = 0, i = 0, k = capacity; 
  while ((k = k-items[indices[i]].w) >= 0 && i < n) {
    res[indices[i]] = 1;
    v += items[indices[i++]].v;
  }
  return pair<int, vector<int>>(v, res);
}

#endif
