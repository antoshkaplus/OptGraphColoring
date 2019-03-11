#pragma once

#include "lk_util.hpp"


template <class Tour>
class LK_Permutation_V1 {

    Tour& tour;
    std::vector<City> ts;
    vector<Index> ts_line_order;
    vector<Index> ts_left;
    vector<bool> ts_visited;

public:
    using tour_type = Tour;

    LK_Permutation_V1(Tour& tour) : tour(tour) {}

    template <typename... TCities>
    LK_Permutation_V1(Tour& tour, TCities... cities) : tour(tour) {
        Assign(cities...);
    }

    City back() const {
        return ts.back();
    }

    City front() const {
        return ts.front();
    }

    Count size() const {
        return ts.size();
    }

    City city(Index index) const {
        return ts[index];
    }

    bool Exists(City city) {
        return std::find(ts.begin(), ts.end(), city) != ts.end();
    }

    void Reset(City c_1, City c_2) {
        ts.resize(2);
        ts[0] = c_1;
        ts[1] = c_2;
    }

    template <typename... TCities>
    void Assign(City c_1, City c_2, TCities... cities) {
        Reset(c_1, c_2);
        Push(cities...);
    }

    template <typename... TCities>
    void Push(City c_1, City c_2, TCities... cities) {
        Push(c_1, c_2);
        Push(cities...);
    }

    void Push(City c_1, City c_2) {
        ts.push_back(c_1);
        ts.push_back(c_2);
    }

    void Pop() {
        ts.pop_back();
        ts.pop_back();
    }

    template <bool kVerbose = false>
    bool CanClose() {
        if (ts.size() % 2 != 0 || ts.size() < 4) return false;

        if constexpr (kVerbose) {
            Println(cout, "ts: ", ts);
        }

        vector<Index> ts_line_order(ts.size());
        iota(ts_line_order.begin(), ts_line_order.end(), 0);

        sort(ts_line_order.begin(), ts_line_order.end(), [&](auto i_1, auto i_2) {
            return tour.LineOrdered(ts[i_1], ts[i_2]);
        });

        if constexpr (kVerbose) {
            Println(cout, ts_line_order);
        }

        // endpoints of paths left

        // for each t_index provided gives back t_index_ of the path that's still there
        vector<Index> ts_left(ts.size());
        auto i = 0;
        if (broken_endpoint(ts_line_order[0], ts.size()) == ts_line_order[1]) {
            i = 1;
            ts_left[ts_line_order[0]] = ts_line_order[ts.size()-1];
            ts_left[ts_line_order[ts.size()-1]] = ts_line_order[0];
        }
        // this comparison proabably needed everywhere as we go
        for (; i < ts.size()-1; i+=2) {
            ts_left[ts_line_order[i]] = ts_line_order[i+1];
            ts_left[ts_line_order[i+1]] = ts_line_order[i];
        }

        if constexpr (kVerbose) {
            Println(cout, ts_left);
        }

        vector<bool> ts_visited(ts.size(), false);

        // checking if we get back to endpoint before
        // passing through all of them
        auto t_index = 0;
        for (auto i = 0; i < ts.size() / 2; ++i) {
            if (ts_visited[t_index]) return false;

            ts_visited[t_index] = true;
            t_index = ts_left[t_index];

            ts_visited[t_index] = true;
            t_index = added_endpoint(t_index, ts.size());
        }
        return true;
    }
};