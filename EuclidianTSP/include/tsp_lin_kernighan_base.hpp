#pragma once

#include "tsp.hpp"


template<class Tour>
class LinKernighanBase {

    Tour& tour;
    const vector<Point>& ps;
    const grid::Grid<Index>& nearestNeighbours;
    double epsilon;

    std::chrono::system_clock::duration time_limit = std::chrono::system_clock::duration::max();

    bool time_limit_reached_ = false;
    int64_t iter_count_ = 0;
    std::vector<Count> close_count_;

public:
    LinKernighanBase(Tour& tour, const vector<Point>& ps, const grid::Grid<Index>& nearestNeighbours, double epsilon)
        : tour(tour), ps(ps), nearestNeighbours(nearestNeighbours), epsilon(epsilon) {}


    void set_time_limit(std::chrono::system_clock::duration time_limit) {
        this->time_limit = time_limit;
    }

    bool time_limit_reached() const {
        return time_limit_reached_;
    }

    auto iter_count() const {
        return iter_count_;
    }

    const auto& close_count() const {
        return close_count_;
    }

    // bottleneck is here unable to get to large K because of it.
    // if only could do it faster than n*log (n) or something
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

    template <bool kVerbose = false>
    void Close(const vector<Index>& ts) {

        close_count_.resize(std::max(ts.size(), close_count_.size()), 0);
        ++close_count_[ts.size()-1];

        auto i_1 = ts[0];
        for (auto i = 1; i < ts.size()-2; i+=2) {
            auto i_2 = ts[i];
            auto i_3 = ts[i+1];
            auto i_4 = ts[i+2];
            tour.Flip(i_1, i_2, i_3, i_4);
        }

    }

    template <bool kVerbose = false>
    void Solve() {

        std::uniform_int_distribution<> distr(0, ps.size() - 1);
        std::default_random_engine rng;

        auto start_timestamp = std::chrono::system_clock::now();

        bool again = true;
        while (again && std::chrono::system_clock::now() - start_timestamp < time_limit) {
            again = false;

            vector<Index> ts;
            ts.push_back(distr(rng));

            for (auto i = 0; i < ps.size(); ++i) {
                // should I try Prev too ?
                ts.push_back(tour.Next(ts.back()));
                ++iter_count_;

                double before = 0; (void)before;
                if constexpr (kVerbose) before = TSP_Distance(ps, tour.Order());

                if (TryImprove<kVerbose>(ts)) {
                    CheckGain<kVerbose>(ts);
                    Close<kVerbose>(ts);

                    if constexpr (kVerbose) {
                        Println(cout, "new tour:");
                        Println(cout, tour.Order());

                        assert(!isFeasibleSolution(ps, tour.Order()));

                        auto after = TSP_Distance(ps, tour.Order());
                        assert(after < before);
                    }

                    again = true;
                    break;
                }

                ts[0] = ts[1];
                ts.resize(1);
            }

            for (auto i = 0; i < ps.size(); ++i) {
                // should I try Prev too ?
                ts.push_back(tour.Prev(ts.back()));
                ++iter_count_;

                double before = 0; (void)before;
                if constexpr (kVerbose) before = TSP_Distance(ps, tour.Order());

                if (TryImprove<kVerbose>(ts)) {
                    CheckGain<kVerbose>(ts);
                    Close<kVerbose>(ts);

                    if constexpr (kVerbose) {
                        Println(cout, "new tour:");
                        Println(cout, tour.Order());

                        assert(!isFeasibleSolution(ps, tour.Order()));

                        auto after = TSP_Distance(ps, tour.Order());
                        assert(after < before);
                    }

                    again = true;
                    break;
                }

                ts[0] = ts[1];
                ts.resize(1);
            }
        }

        if (std::chrono::system_clock::now() - start_timestamp > time_limit) {
            time_limit_reached_ = true;
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
    void CheckGain(const vector<Index>& ts) {
        if constexpr (kVerbose) {
            Println(cout, "improve");
            Println(cout, ts);
            double total = 0;
            for (auto i = 1; i < ts.size(); ++i) {
                auto d = 1;
                if (i%2 == 1) d =-1;
                total += d*Distance(ts[i-1], ts[i]);
                Println(cout, "total: ", total);
            }
            Println(cout, "total: ", total + Distance(ts[0], ts.back()));
        }
    }

    template <bool kVerbose>
    bool TryImprove(vector<Index>& ts, double gain) {
        if constexpr (kVerbose) {
            Println(cout, "try improve: ", " gain: ", gain);
            Println(cout, ts);
        }

        // this shit is actually should be recursive probably
        auto row = ts.back();

        auto try_count = nearestNeighbours.col_count();
        // the bigger the number the better the result
        if (ts.size() >= 60) try_count = 1;

        for (auto col = 0; col < try_count; ++col) {
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
                    Println(cout, "can close, close gain: ", closeGain, " , continue: ", newGain);
                }

                if (closeGain < 0) {
                    return true;
                }

                if (TryImprove<kVerbose>(ts, newGain)) {
                    return true;
                }

                ts.pop_back();
                ts.pop_back();
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

