#pragma once

#include "support.h"

class Knapsack {
    vector<int> t_arr;
    int t_v, t_w;
    int r_v, r_w;
    vector<int> best_arr;
    int best_v, best_w;
    int n, k;
    vector<int> w, v; // sorted by effectiveness

    void take(int i, int j) {
        if (t_w + w[i] <= k) {
            t_w += w[i];
            t_v += v[i];
            t_arr[i] = 1;
            go(i + 1, j);
            t_w -= w[i];
            t_v -= v[i];
        }
    }

    void skip(int i, int j) {
        int r_w_old = r_w;
        int r_v_old = r_v;
        r_w -= w[i];
        r_v -= v[i];
        while (j < n && r_w < k) {
            r_w += w[j];
            r_v += v[j];
            j++;
        }
        if (r_v > best_v) {
            t_arr[i] = 0;
            go(i + 1, j);
        }
        r_w = r_w_old;
        r_v = r_v_old;
    }

    void go(int i, int j) {
        if (i == n) {
            if (t_v > best_v) {
                best_arr = t_arr;
                best_w = t_w;
                best_v = t_v;
//                cout << "sol: " << best_v << endl;
            }
            return;
        }
        take(i, j);
        skip(i, j);
    }


public:
    pair<int, vector<int>> relaxation(const vector<Item>& items, int capacity) {
        vector<int> res(items.size());
        n = (int) items.size();
        k = capacity;

        vector<double> keys(n);
        for (int i = 0; i < n; i++) {
            keys[i] = 1. * items[i].v / items[i].w;
        }
        vector<int> indices(n);
        for (int i = 0; i < n; i++) indices[i] = i;
        sort(indices.begin(), indices.end(), [&keys](int i1, int i2) { return keys[i1] > keys[i2]; });

        w.resize(n);
        v.resize(n);
        for (int i = 0; i < n; i++) {
            w[i] = items[indices[i]].w;
            v[i] = items[indices[i]].v;
        }
        best_v = -1;
        r_w = 0;
        r_v = 0;
        int i = 0;
        while (i < n && r_w < k) {
            r_w += w[i];
            r_v += v[i];
            i++;
        }
        t_w = 0;
        t_v = 0;
        t_arr.resize(n, 0);

        go(0, i);

        for (int i = 0; i < n; i++) {
            res[indices[i]] = best_arr[i];
        }
        return pair<int, vector<int>>(best_v, res);
    }

};