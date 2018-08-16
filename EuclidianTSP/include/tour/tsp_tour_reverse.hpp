#pragma once

#include "tsp_util.hpp"


// goes always middle
class Updater_1 {
    Index a, b;

public:
    Updater_1() {}
    Updater_1(Index a, Index b) : a(a), b(b) {
        assert(a <= b);
    }

    void Next() {
        assert(a < b);
        ++a;
        --b;
    }

    pair<Index, Index> values() const {
        return {a, b};
    };

    bool IsFinished() const {
        return a >= b;
    }
};

// always goes to the sides
class Updater_2 {
    Index a, b;
    Count cityCount;
    Count iters;

    Index Prv(Index a, Count cityCount) {
        return prev_ring_index(a, cityCount);
    }
    Index Nxt(Index b, Count cityCount) {
        return next_ring_index(b, cityCount);
    }

public:
    Updater_2() {}
    Updater_2(Index a, Index b, Count cityCount)
            : a(Prv(a, cityCount)), b(Nxt(b, cityCount)), cityCount(cityCount) {

        // first iter always comes without changes
        iters = (a + 1 + cityCount - b) / 2 - 1;
    }

    void Next() {
        --iters;
        a = Prv(a, cityCount);
        b = Nxt(b, cityCount);
    }

    pair<Index, Index> values() const {
        return {a, b};
    };

    bool IsFinished() const {
        return iters <= 0;
    }
};

// goes to the sides after certain threshold reached
class UpdaterCombine {
    Updater_1 updater_1;
    Updater_2 updater_2;
    bool use_1;
public:
    UpdaterCombine(Index a, Index b, Count cityCount, double insideThreshhold) {
        use_1 = (1. * (b-a+1) / cityCount < insideThreshhold);
        if (use_1) updater_1 = Updater_1(a, b);
        else updater_2 = Updater_2(a, b, cityCount);
    }

    void Next() {
        use_1 ? updater_1.Next() : updater_2.Next();
    };

    pair<Index, Index> values() const {
        return use_1 ? updater_1.values() : updater_2.values();
    };

    bool IsFinished() const {
        return use_1 ? updater_1.IsFinished() : updater_2.IsFinished();
    }
};

class Updater_1_Factory {
public:
    Updater_1 Create(Index a, Index b) {
        return {a, b};
    }
};

class Updater_2_Factory {
    Count cityCount;

public:
    Updater_2_Factory(Count cityCount) : cityCount(cityCount) {
        assert(cityCount != 0);
    }

    Updater_2 Create(Index a, Index b) {
        return {a, b, cityCount};
    }
};

class UpdaterCombineFactory {
    double insideThreshhold;
    Count cityCount;

public:
    UpdaterCombineFactory(Count cityCount, double insideThreshhold)
            : insideThreshhold(insideThreshhold), cityCount(cityCount) {

        assert(cityCount != 0);
    }

    UpdaterCombine Create(Index a, Index b) {
        return {a, b, cityCount, insideThreshhold};
    }
};

// returns if the the whole tour should be considered reversed
template<class Swap>
bool Reverse(Index a, Index b, Count length, Swap&& swapElems, double threshold=0.5) {
    if (a == b) return false;

    bool insideReverseRequested = true;
    if (a > b) {
        insideReverseRequested = false;
        swap(a, b);
    }

    auto R = [&](auto&& u) {
        while (!u.IsFinished()) {
            auto [i_1, i_2] = u.values();
            swapElems(i_1, i_2);
            u.Next();
        }
    };

    if ((b - a + 1.) / length < threshold) {
        // reverse as is
        R(Updater_1(a, b));
        return insideReverseRequested;
    } else {
        R(Updater_2(a, b, length));
        return !insideReverseRequested;
    }
}

