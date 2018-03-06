//
//  KnapsackOpt.h
//  Knapsack
//
//  Created by Anton Logunov on 6/20/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef Knapsack_KnapsackOpt_h
#define Knapsack_KnapsackOpt_h

#include "support.h"

class KnapsackOpt {
  
  vector<int> t_arr;
  int t_v, t_w;
  
  int r_w;
  double r_v;
  
  vector<int> best_arr;
  int best_v, best_w;
  
  int n, k;
  
  vector<int> w, v;
  
  void take(int i, int j, int sz) {
    if (t_w + w[i] <= k) {
      t_w += w[i];
      t_v += v[i];
      t_arr[i] = 1;
      go(i+1, j, sz);
      t_w -= w[i];
      t_v -= v[i];
    }
  }
  
  void skip(int i, int j, int sz) {
    int r_w_old = r_w;
    double r_v_old = r_v;
    // always free partial size first
    if (j < n) {
      r_w -= sz;
      r_v -= 1.*sz*v[j]/w[j];
    }
    if (i == j) {
      //cout << "hoh" << endl;
      j++;
    }
    else {
      r_w -= w[i];
      r_v -= v[i];
    }
    while (j < n && r_w + w[j] <= k) {
      r_w += w[j];
      r_v += v[j];
      j++;
    }
    sz = 0;
    if (j < n) {
      sz = k - r_w;
      r_w = k;
      r_v += 1.*sz*v[j]/w[j];
    }
    if (r_v > best_v) {
      t_arr[i] = 0;
      go(i+1, j, sz);
    }
    r_w = r_w_old;
    r_v = r_v_old;
  }
  
  void go(int i, int j, int sz) {
    if (i == n) {
      if (t_v > best_v) {
        best_arr = t_arr;
        best_w = t_w;
        best_v = t_v;
        cout << "sol: " << best_v << endl;
      }
      return; 
    }
    take(i, j, sz);
    skip(i, j, sz); 
  }
  
  
public:
  pair<int, vector<int>> relaxation(const vector<Item>& items, int capacity) {
    vector<int> res(items.size());
    n = (int)items.size();
    k = capacity;
    
    vector<double> keys(n);
    for (int i = 0; i < n; i++) {
      keys[i] = 1.*items[i].v/items[i].w;
    }
    vector<int> indices(n);
    for (int i = 0; i < n; i++) indices[i] = i;
    sort(indices.begin(), indices.end(), [&keys](int i1, int i2){ return keys[i1] > keys[i2]; });
    
    w.resize(n);
    v.resize(n);
    for (int i = 0; i < n; i++) {
      w[i] = items[indices[i]].w;
      v[i] = items[indices[i]].v;
    }
    best_v = -1;
    int i = 0;
    r_w = 0;
    r_v = 0.;
    while (i < n && r_w + w[i] <= k) {
      r_w += w[i];
      r_v += v[i];
      i++;
    }
    int sz = 0;
    if (i < n) {
      sz = k - r_w;
      r_w = k;
      r_v += 1.*sz*v[i]/w[i];
    }
    t_w = 0;
    t_v = 0;
    t_arr.resize(n, 0);
    
    go(0, i, sz);
    
    for (int i = 0; i < n; i++) {
      res[indices[i]] = best_arr[i];
    }
    return pair<int, vector<int>>(best_v, res);
  }
  
};


#endif
