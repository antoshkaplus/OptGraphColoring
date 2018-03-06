#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct Item {
    int w;
    int v;
};

pair<int, vector<int>> usual(const vector<Item> &items, int capacity) {
    Count itemCount = items.size();

    vector<int> res(itemCount);
    vector<int> buffer((capacity + 1) * (itemCount + 1));

    // column 0 : 0 items
    for (int i = 0; i < capacity + 1; i++) {
        buffer[i * (itemCount + 1)] = 0;
    }
    // row 0 : 0 capacity
    for (int i = 0; i < itemCount + 1; i++) {
        buffer[i] = 0;
    }
    // loop by items
    for (int i = 1; i < itemCount + 1; i++) {
        const Item &t = items[i];
        // loop by capacity
        for (int j = 0; j < min(t.w, capacity + 1); j++) {
            buffer[j * (n + 1) + i] = buffer[j * (itemCount + 1) + i - 1];
        }
        for (int j = t.w; j < capacity + 1; j++) {
            buffer[j * (itemCount + 1) + i] = max(buffer[j * (itemCount + 1) + i - 1], buffer[(j - t.w) * (itemCount + 1) + i - 1] + t.v);
        }
    }

    int w = capacity;
    for (int i = itemCount; i > 0; i--) {
        if (buffer[w * (itemCount + 1) + i] == buffer[w * (itemCount + 1) + i - 1]) {
            res[i] = 0;
        } else {
            res[i] = 1;
            w -= items[i].w;
        }
    }

    return pair<int, vector<int>>(buffer[(k + 1) * (n + 1) - 1], res);
}

pair<int, vector<int>> greedy(const vector<Item> &items, int capacity) {
    vector<int> res(items.size());
    int n = (int) items.size();
    vector<double> keys(n);
    for (int i = 0; i < n; i++) {
        keys[i] = 1. * items[i].v / items[i].w;
    }
    vector<int> indices(n);
    for (int i = 0; i < n; i++) indices[i] = i;
    std::sort(indices.begin(), indices.end(), [&keys](int i1, int i2) { return keys[i1] > keys[i2]; });

    int v = 0, i = 0, k = capacity;
    while ((k = k - items[indices[i]].w) >= 0 && i < n) {
        res[indices[i]] = 1;
        v += items[indices[i++]].v;
    }
    return pair<int, vector<int>>(v, res);
}