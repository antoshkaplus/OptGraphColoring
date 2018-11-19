#pragma once

#include "tsp.hpp"


template<class Tour>
class LinKernighanBase {

    Tour& tour;
    const vector<Point>& ps;
    const grid::Grid<Index>& nearestNeighbours;
    double epsilon;

public:
    LinKernighanBase(Tour& tour, const vector<Point>& ps, const grid::Grid<Index>& nearestNeighbours, double epsilon)
        : tour(tour), ps(ps), nearestNeighbours(nearestNeighbours), epsilon(epsilon) {}

    template <bool kVerbose = false>
    bool CanClose(const vector<Index>& ts) {
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

    // again we have ts vector + tour. need to close it.
    template <bool kVerbose = false>
    void Close(const vector<Index>& ts) {
        // can change

        if constexpr (kVerbose) {
            Println(cout, "start new");
        }

        for (auto i = 0; i < ts.size()-1; i+=2) {
            auto i_1 = ts[i];
            auto i_2 = ts[i+1];
            auto i_3 = ts[(i+2) % ts.size()];

            if constexpr (kVerbose) {
                Println(cout, i_1, " ", i_2, " ", i_3);
            }

            if constexpr (kVerbose) {
                Println(cout, "pre order: ", tour.OrderToCity());
            }

            if (tour.Prev(i_1) == i_2) {
                tour.Reverse(i_1, i_3);
            } else if (tour.Next(i_1) == i_2) {
                tour.Reverse(i_3, i_1);
            }

            if constexpr (kVerbose) {
                Println(cout, tour.OrderToCity());
            }
        }
    }

    template <bool kVerbose = false>
    void Solve() {

        std::uniform_int_distribution<> distr(0, ps.size() - 1);
        std::default_random_engine rng;

        start:
        vector<Index> ts;
        ts.push_back(distr(rng));

        for (auto i = 0; i < ps.size(); ++i) {
            // should I try Prev too ?
            ts.push_back(tour.Next(ts.back()));

            if (TryImprove<kVerbose>(ts)) {
                Close(ts);
                if constexpr (kVerbose) {
                    Println(cout, "new tour:");
                    Println(cout, tour.Order());
                }
                goto start;
            }

            ts[0] = ts[1];
            ts.resize(1);
        }
    }

    template <bool kVerbose>
    bool TryImprove(vector<Index>& ts) {
        assert(ts.size() == 2);

        // how big by absolute value negative value is
        double gain = -Distance(ts[0], ts[1]);
        if constexpr (kVerbose) {
            Println(cout, ts[0], " ", ts[1], " gain: ", gain);
        }

        return TryImprove<kVerbose>(ts, gain);
    }

private:

    template <bool kVerbose>
    bool TryImprove(vector<Index>& ts, double gain) {
        if constexpr (kVerbose) {
            Println(cout, "try improve: ", " gain: ", gain);
            Println(cout, ts);
        }

        // this shit is actually should be recursive probably
        auto row = ts.back();
        for (auto col = 0; col < nearestNeighbours.col_count(); ++col) {
            // city will be added
            auto city = nearestNeighbours(row, col);

            if (tour.Prev(city) == ts.back() || tour.Next(city) == ts.back()) continue;
            if (find(ts.begin(), ts.end(), city) != ts.end()) continue;

            double gain_2 = gain + Distance(ts.back(), city);
            if (gain_2 > 0) continue;

            if constexpr (kVerbose) {
                Println(cout, "add edge: ", city , " gain: ", gain_2);
            }

            array<Index, 2> ends = {{tour.Next(city), tour.Prev(city)}};

            for (auto i : {0, 1}) {
                //  this is wrong. has to consider whole edges
                if (find(ts.begin(), ts.end(), ends[i]) != ts.end()) continue;

                // auto new_x = make_pair(city, ends[i]);

                if constexpr (kVerbose) {
                    Println(cout, "consider broken: ", city, " ", ends[i]);
                }

                ts.push_back(city);
                ts.push_back(ends[i]);
                if (!CanClose(ts)) {
                    ts.pop_back();
                    ts.pop_back();

                    if constexpr (kVerbose) {
                        Println(cout, "can't close");
                    }

                    continue;
                }

                double newGain = gain_2 - Distance(city, ends[i]);
                double closeGain = newGain + Distance(ends[i], ts[0]);

                if constexpr (kVerbose) {
                    Println(cout, "can close, close gain: ", closeGain);
                }

                if (closeGain < 0) {
                    return true;
                }

                if (TryImprove<kVerbose>(ts, newGain)) {
                    return true;
                }
            }
        }

        return false;
    }

    double Distance(Index i_1, Index i_2) const {
        return TSP_Distance(ps, i_1, i_2);
    }

    // y : returns another endpoint of added t_i
    inline static Index added_endpoint(Index t_index, Count t_sz) {
        return (t_index % 2 == 0) ? (t_index + t_sz - 1) % t_sz : (t_index+1) % t_sz;
    }

    // x : returns another endpoint of broken t_i
    inline static Index broken_endpoint(Index t_index, Count t_sz) {
        return (t_index % 2 == 0) ? (t_index + t_sz + 1) % t_sz :  t_index-1;
    }

};

