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

    class OptSet {

        using Set = std::set<Index, std::function<bool(Index, Index)>>;

        std::vector<Index> cities_;
        Set ordered_{[](Index, Index) {return true;}};
    public:

        OptSet(Tour& tour) {
            ordered_ = Set([&](City c_1, City c_2) {
                return tour.LineOrdered(c_1, c_2);
            });
        }

        void Push(City city) {
            cities_.push_back(city);
            ordered_.insert(city);
        }

        void Pop() {
            ordered_.erase(cities_.back());
            cities_.pop_back();
        }

        const vector<City>& cities() const {
            return cities_;
        }

        const Set& ordered() const {
            return ordered_;
        }

        void ToOne() {
            cities_[0] = cities_[1];
            cities_.resize(1);

            ordered_.clear();
            ordered_.insert(cities_.back());
        }

        City LinePrev(City c_1) const {
            auto it = ordered_.find(c_1);
            if (it == ordered_.begin()) {
                it = ordered_.end();
            }
            return *std::prev(it);
        }

        City LineNext(City c_1) const {
            auto it = std::next(ordered_.find(c_1));
            if (it == ordered_.end()) {
                return *ordered_.begin();
            }
            return *it;
        }
    };

    struct PopGuard {
        OptSet& s;
        bool skip_ = false;

        PopGuard(OptSet& s) : s(s) {}
        ~PopGuard() { if (!skip_) s.Pop(); }

        void skip() { skip_ = true; }
    };

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

    bool ValidAdd(const OptSet& ts)
    {
        if (ts.cities().size() <= 3) return true;

        auto& cs = ts.cities();

        auto last = cs.back();
        auto first = cs[cs.size()-2];

        Println(cout, first, " ", last, " ", ts.LinePrev(last), " ", ts.LineNext(last));
        if (ts.LinePrev(last) == first || ts.LineNext(last) == first) {
            return false;
        }

        return true;
    }

    bool CanClose(const OptSet& ts) {

        auto& cs = ts.cities();

        auto last = cs.back();
        auto first = cs.front();

        Println(cout, first, " ", last, " ", ts.LinePrev(last), " ", ts.LineNext(last));
        if (ts.LinePrev(last) == first || ts.LineNext(last) == first) {
            return false;
        }

        if (ts.cities().size() == 4) return true;


        {
            // pre broken edge
            auto c_1 = cs[cs.size()-2];
            auto c_2 = cs[cs.size()-3];
            // c_1, c_2 - added edge
            // c_1, last - last broken edge

            // find
            auto a_1 = ts.LineNext(c_1);
            auto a_2 = ts.LinePrev(c_1);

            auto another_c_1 = a_1 == last ? a_2 : a_1;

            auto b_1 = ts.LineNext(c_2);
            auto b_2 = ts.LinePrev(c_2);

            auto another_c_2 = b_1 == cs[cs.size()-4] ? b_2 : b_1;

            auto another_c_1_index = 0;
            while (cs[another_c_1_index] != another_c_1) ++another_c_1_index;

            if (cs[added_endpoint(another_c_1_index, cs.size())] == another_c_2) {
                Println(cout, "lol2 ", c_1, " ", c_2, " ", cs[added_endpoint(another_c_1_index, cs.size())], " ", another_c_2);
                return false;
            }
        }
        {
            City anotherLast;
            {
                auto c_1 = ts.LinePrev(last);
                auto c_2 = ts.LineNext(last);
                anotherLast = (c_1 == cs[cs.size() - 2] ? c_2 : c_1);
            }
            City anotherFirst;
            {
                auto c_1 = ts.LinePrev(first);
                auto c_2 = ts.LineNext(first);
                anotherFirst = (c_1 == cs[1] ? c_2 : c_1);
            }

            // keep indices around
            auto anotherFirstIndex = 0;
            while (cs[anotherFirstIndex] != anotherFirst) ++anotherFirstIndex;

            if (cs[added_endpoint(anotherFirstIndex, cs.size())] == anotherLast) {
                Println(cout, "lol ", anotherLast, " ", anotherFirst,
                        " next: ", cs[(anotherFirstIndex + 1) % cs.size()],
                        " prev: ", cs[(anotherFirstIndex + cs.size() - 1) % cs.size()]);
                return false;
            }
        }

        return true;

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

            OptSet ts(tour);
            ts.Push(distr(rng));

            for (auto i = 0; i < ps.size(); ++i) {
                // should I try Prev too ?
                ts.Push(tour.Next(ts.cities().back()));
                ++iter_count_;

                double before = 0; (void)before;
                if constexpr (kVerbose) before = TSP_Distance(ps, tour.Order());

                if (TryImprove<kVerbose>(ts)) {
                    CheckGain<kVerbose>(ts.cities());
                    Close<kVerbose>(ts.cities());

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

                ts.ToOne();
            }

            // bring back prev guy
        }

        if (std::chrono::system_clock::now() - start_timestamp > time_limit) {
            time_limit_reached_ = true;
        }
    }

    template <bool kVerbose>
    bool TryImprove(OptSet& ts) {
        assert(ts.cities().size() == 2);

        // how big by absolute value negative value is
        double gain = -Distance(ts.cities()[0], ts.cities()[1]);
        if constexpr (kVerbose) {
            Println(cout, ts.cities()[0], " ", ts.cities()[1], " gain: ", gain);
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
    bool TryImprove(OptSet& ts, double gain) {
        if constexpr (kVerbose) {
            Println(cout, "try improve: ", " gain: ", gain);
            Println(cout, ts.cities());
        }

        // this shit is actually should be recursive probably
        auto row = ts.cities().back();

        auto try_count = nearestNeighbours.col_count();
        // the bigger the number the better the result
        if (ts.cities().size() >= 60) try_count = 1;

        for (auto col = 0; col < try_count; ++col) {
            // city will be added
            auto city = nearestNeighbours(row, col);

            // could insert row here
            if (tour.Prev(city) == ts.cities().back() || tour.Next(city) == ts.cities().back()) continue;
            if (ts.ordered().count(city) == 1) continue;

            double gain_2 = gain + Distance(ts.cities().back(), city);
            if (gain_2 > 0) continue;

            if constexpr (kVerbose) {
                Println(cout, "add edge: ", city , " gain: ", gain_2);
            }

            ts.Push(city);
            PopGuard guard(ts);
            if (!ValidAdd(ts)) continue;

            array<Index, 2> ends = {{tour.Next(city), tour.Prev(city)}};
            for (auto i : {0, 1}) {
                //  this is wrong. has to consider whole edges
                if (ts.ordered().count(ends[i]) == 1) continue;

                // auto new_x = make_pair(city, ends[i]);

                if constexpr (kVerbose) {
                    Println(cout, "consider broken: ", city, " ", ends[i]);
                }

                ts.Push(ends[i]);
                PopGuard end_guard(ts);

                Println(cout, "order: ", tour.Order());
                Println(cout, "ordered ", std::vector<City>{ts.ordered().begin(), ts.ordered().end()});
                Println(cout, "set: ", ts.cities());
                Println(cout, "old: ", CanClose(ts.cities()), " new: ", CanClose(ts));
                assert(CanClose(ts.cities()) == CanClose(ts));

                if (!CanClose(ts)) {

                    if constexpr (kVerbose) {
                        Println(cout, "can't close");
                    }

                    continue;
                }

                double newGain = gain_2 - Distance(city, ends[i]);
                double closeGain = newGain + Distance(ends[i], ts.cities()[0]);

                if constexpr (kVerbose) {
                    Println(cout, "can close, close gain: ", closeGain, " , continue: ", newGain);
                }

                if (closeGain < 0) {
                    guard.skip();
                    end_guard.skip();
                    return true;
                }

                if (TryImprove<kVerbose>(ts, newGain)) {
                    guard.skip();
                    end_guard.skip();
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

