#pragma once

#include "tsp.hpp"
#include "lk_interface.hpp"
#include "lk_permutation.hpp"


template<typename Tour, typename Permutation=LK_Permutation<Tour>>
class LK_Base : public LK_Interface {

    Tour& tour;
    const vector<Point>& ps;
    const grid::Grid<Index>& nearestNeighbours;
    double epsilon;

    Duration time_limit = Duration::max();

    Count max_perm_depth_ = 10;
    int64_t iter_count_ = 0;
    std::vector<Count> close_count_;

    struct PopGuard {
        Permutation& s;
        bool skip_ = false;

        PopGuard(Permutation& s) : s(s) {}
        ~PopGuard() { if (!skip_) s.Pop(); }

        void skip() { skip_ = true; }
    };

    std::default_random_engine rng;

    Permutation permutation;
    Duration time_spent_;

public:
    LK_Base(Tour& tour, const vector<Point>& ps, const grid::Grid<Index>& nearestNeighbours, double epsilon)
        : tour(tour), ps(ps), nearestNeighbours(nearestNeighbours), epsilon(epsilon), permutation(tour) {}


    void set_time_limit(Duration time_limit) {
        this->time_limit = time_limit;
    }

    bool time_limit_reached() const override {
        return time_spent_ > time_limit;
    }

    void set_max_perm_depth(Count value) {
        max_perm_depth_ = value;
    }

    int64_t iter_count() const override {
        return iter_count_;
    }

    const vector<Count>& close_count() const override {
        return close_count_;
    }

    Duration time_spent() const override {
        return time_spent_;
    }

    vector<City> Solve() override {
        return solve();
    }

    template <bool kVerbose = false>
    vector<City> solve() {

        std::uniform_int_distribution<> distr(0, ps.size() - 1);

        auto start_timestamp = std::chrono::system_clock::now();

        bool again = true;
        while (again && std::chrono::system_clock::now() - start_timestamp < time_limit) {
            again = false;

            auto city_1 = distr(rng);
            bool next = distr(rng) % 2 == 0;

            for (auto i = 0; i < ps.size(); ++i) {
                // should I try Prev too ?
                auto city_2 = next ? tour.Next(city_1) : tour.Prev(city_1);
                permutation.Reset(city_1, city_2);
                ++iter_count_;

                double before = 0; (void)before;
                if constexpr (kVerbose) before = TSP_Distance(ps, tour.Order());

                if (TryImprove<kVerbose>(permutation)) {
//                    CheckGain<kVerbose>(permutation.cities());
                    Close<kVerbose>(permutation);

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

                city_1 = city_2;
            }
        }

        time_spent_ = std::chrono::system_clock::now() - start_timestamp;

        return tour.Order();
    }

    template <bool kVerbose>
    bool TryImprove(Permutation& perm) {
        assert(perm.size() == 2);

        // how big by absolute value negative value is
        double gain = -Distance(perm.city(0), perm.city(1));
        if constexpr (kVerbose) {
            Println(cout, perm.city(0), " ", perm.city(1), " gain: ", gain);
        }

        return TryImprove<kVerbose>(perm, gain);
    }

    template < bool kVerbose = false>
    void Close(const Permutation& perm) {

        close_count_.resize(std::max<Count>(perm.size(), close_count_.size()), 0);
        ++close_count_[perm.size()-1];

        ::Close(tour, permutation);
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
    bool TryImprove(Permutation& perm, double gain) {

        if constexpr (kVerbose) {
            Println(cout, "try improve: ", " gain: ", gain);
//            Println(cout, ts);
        }

        // this shit is actually should be recursive probably
        auto row = perm.back();

        auto try_count = nearestNeighbours.col_count();
        // the bigger the number the better the result
        if (perm.size() >= max_perm_depth_) try_count = 1;

        for (auto col = 0; col < try_count; ++col) {
            // city will be added
            auto city = nearestNeighbours(row, col);

            // could insert row here
            if (tour.Prev(city) == perm.back() || tour.Next(city) == perm.back()) continue;
            if (perm.Exists(city)) continue;

            double gain_2 = gain + Distance(perm.back(), city);
            if (gain_2 > 0) continue;

            if constexpr (kVerbose) {
                Println(cout, "add edge: ", city , " gain: ", gain_2);
            }

            array<Index, 2> ends = {{tour.Next(city), tour.Prev(city)}};
            if (std::uniform_real_distribution<>()(rng) > 0.5) std::swap(ends[0], ends[1]);

            for (auto i : {0, 1}) {
                //  this is wrong. has to consider whole edges
                if (perm.Exists(ends[i])) continue;

                // auto new_x = make_pair(city, ends[i]);

                if constexpr (kVerbose) {
                    Println(cout, "consider broken: ", city, " ", ends[i]);
                }

                perm.Push(city, ends[i]);
                PopGuard guard(perm);

                if (!permutation.CanClose()) {

                    if constexpr (kVerbose) {
                        Println(cout, "can't close");
                    }

                    continue;
                }

                double newGain = gain_2 - Distance(city, ends[i]);
                double closeGain = newGain + Distance(ends[i], perm.front());

                if constexpr (kVerbose) {
                    Println(cout, "can close, close gain: ", closeGain, " , continue: ", newGain);
                }

                if (closeGain < 0) {
                    guard.skip();
                    return true;
                }

                if (TryImprove<kVerbose>(perm, newGain)) {
                    guard.skip();
                    return true;
                }
            }
        }

        return false;
    }

    double Distance(Index i_1, Index i_2) const {
        return TSP_Distance(ps, i_1, i_2);
    }
};

