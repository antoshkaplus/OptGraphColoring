#pragma once

#include "tsp.hpp


template<class Tour>
class LinKernighanBase {

    Tour& tour;

public:
    LinKernighanBase(Tour& tour) : tour(tour) {}

    bool CanClose(const vector<Index>& ts) {
        vector<Index> ts_order(ts.size());
        iota(ts_order.begin(), ts_order.end(), 0);

        sort(ts_order.begin(), ts_order.end(), [](auto i_1, auto i_2) {
            tour.LineOrdered(ts[i_1], ts[i_2]);
        });

        // endpoints of paths left
        vector<Index> ts_left(ts.size());
        auto i = 0;
        if (broken_endpoint(ts_order[0] == ts_order[1]) {
            i = 1;
            ts_left[ts_order[0]] = ts_order[ts.size()-1];
            ts_left[ts_order[ts.size()-1]] = ts_order[0];
        }
        for (; i < ts.size()-1; i+=2) {
            ts_left[ts_order[i]] = ts_order[i+1];
            ts_left[ts_order[i+1]] = ts_order[i];
        }


        vector<bool> ts_visited(ts.size(), false);

        auto t_index = 0;
        for (auto i = 0; i < ts.size() / 2; ++i) {
            if (ts_visited[t_index]) return false;

            ts_visited[t_index] = true;
            t_index = ts_left[t_index];
            ts_visited[t_index] = true;
            t_index = added(t_index);
        }
        return true;
    }

    // again we have ts vector + tour. need to close it.
    void Close(const vector<Index>& ts) {
        // can change
        auto first = ts[0];
        for (auto i = 0; i < ts.size()-2; i+=2) {
            auto i_2 = ts[i+1];
            auto i_3 = ts[i+2];

            if (tour.Between(first, i_2, i_3)) {
                tour.Reverse(i_3, first);

                if (tour.Prev(i_3) == ts[broken(i+2)]) {
                    first = i_3;
                }
            }
        }
    }

    template<class ForEachNearest>
    void Solve(ForEachNearest&& for_each_nearest) {

        std::uniform_int_distribution<> distr(0, ps.size() - 1);
        std::default_random_engine rng;

        auto tour_distance = TSP_Distance(ps, tour.Order());
        auto epsilon = swap_distance_factor * tour_distance;

        vector<bool>

        start:

        vector<Index> ts;
        ts.push_back(distr(rng));

        for (auto i = 0; i < ps.size(); ++i) {
            ts.push_back(ts.back());
            // look up five closest points
            for_each_nearest(ts.back(), 5, [&](Index city) {
                if (find(ts.begin(), ts.end(), city) != ts.end()) return LoopControl::Continue;

                // compute gain for each
                array<double, 2> gain = {-1, -1};
                array<Index, 2> ends = {{tour.Next(city), tour.Prev(city)}};

                double best_gain = 0;
                Index end;
                for (auto i : {0, 1}) {
                    if ()
                }



            })


            while (b2 != first) {
                if (TSP_Distance(ps, a1, a2) + TSP_Distance(ps, b1, b2) >
                    TSP_Distance(ps, a1, b1) + TSP_Distance(ps, a2, b2) + epsilon) {
                    tour.Flip(a1, a2, b1, b2);
                    goto start;
                }
                b1 = b2;
                b2 = tour.Next(b1);
            }

            ts[0] = ts[1];
            ts.resize(1);
        }


    }

private:

    // y : returns another endpoint of added t_i
    inline static Index added_endpoint(Index t_index) {
        return (i % 2 == 0) ? (i + ts.size() - 1) % ts.size() : i+1
    }

    // x : returns another endpoint of broken t_i
    inline static Index broken_endpoint(Index t_index) {
        return (i % 2 == 0) ? (i + ts.size() + 1) % ts.size() : i-1
    }

};

