#pragma once

#include "support.h"

class KS_BranchBoundFraction {

    vector<int> current_arr;
    int current_v, current_w;

    int bound_w;
    double bound_v;

    vector<int> best_arr;
    int best_v, best_w;

    int n, k;

    vector<int> w, v;

    void take(int i, int j, int sz) {
        if (current_w + w[i] <= k) {
            current_w += w[i];
            current_v += v[i];
            current_arr[i] = 1;
            go(i + 1, j, sz);
            current_w -= w[i];
            current_v -= v[i];
        }
    }

    // todo can i == j really ???
    void skip(int i, int j, int sz) {
        int bound_w_old = bound_w;
        double bound_v_old = bound_v;
        // always free partial size first
        if (j < n) {
            bound_w -= sz;
            bound_v -= 1. * sz * v[j] / w[j];
        }
        if (i == j) {
            //cout << "hoh" << endl;
            j++;
        } else {
            bound_w -= w[i];
            bound_v -= v[i];
        }
        while (j < n && bound_w + w[j] <= k) {
            bound_w += w[j];
            bound_v += v[j];
            j++;
        }
        sz = 0;
        if (j < n) {
            sz = k - bound_w;
            bound_w = k;
            bound_v += 1. * sz * v[j] / w[j];
        }
        if (bound_v > best_v) {
            current_arr[i] = 0;
            go(i + 1, j, sz);
        }
        bound_w = bound_w_old;
        bound_v = bound_v_old;
    }

    void go(int i, int j, int sz) {
        if (i == n) {
            if (current_v > best_v) {
                best_arr = current_arr;
                best_w = current_w;
                best_v = current_v;
            }
            return;
        }
        take(i, j, sz);
        skip(i, j, sz);
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
        int i = 0;
        bound_w = 0;
        bound_v = 0.;
        while (i < n && bound_w + w[i] <= k) {
            bound_w += w[i];
            bound_v += v[i];
            i++;
        }
        int sz = 0;
        if (i < n) {
            sz = k - bound_w;
            bound_w = k;
            bound_v += 1. * sz * v[i] / w[i];
        }
        current_w = 0;
        current_v = 0;
        current_arr.resize(n, 0);

        go(0, i, sz);

        for (int i = 0; i < n; i++) {
            res[indices[i]] = best_arr[i];
        }
        return pair<int, vector<int>>(best_v, res);
    }

};