#pragma once

#include "tsp_two_level_tree_base.hpp"


class TwoLevelTreeTour {

    TwoLevelTreeBase base_;
    Count sz_bound;

public:
    TwoLevelTreeTour(const vector<Index>& cities) : base_(cities), sz_bound(static_cast<Count>(sqrt(cities.size()))) {
        base_.ForEachParent([&](auto it) {
            while (SplitOversized(it));
        });
    }

    TwoLevelTreeTour(Count city_count) : TwoLevelTreeTour(city_count, static_cast<Count>(sqrt(city_count))) {}

    TwoLevelTreeTour(Count city_count, Count sz_bound) : base_(city_count), sz_bound(sz_bound) {
        base_.ForEachParent([&](auto it) {
            while (SplitOversized(it));
        });
    }

    Index Next(Index city) const {
        return base_.Next(city);
    }

    Index Prev(Index city) const {
        return base_.Prev(city);
    }

    bool Between(Index a, Index b, Index c) const {
        return base_.Between(a, b, c);
    }

    bool ReverseOrdered(Index a, Index b) const {
        return base_.seg_ordered(a, b);
    }

    void Flip(Index a, Index b, Index c, Index d) {
        if (a == Next(b)) swap(a, b);
        if (c == Next(d)) swap(c, d);

        auto [r_1, r_2] = std::make_pair(b, c);
        if (!ReverseOrdered(r_1, r_2)) {
            tie(r_1, r_2) = std::make_pair(d, a);
        }

        Reverse(r_1, r_2);
    }

    void Reverse(Index a, Index b) {
        if (!base_.seg_ordered(a, b)) {
            swap(a, b);
        }

        auto a_p = base_.parent(a);
        auto b_p = base_.parent(b);

        if (a_p == b_p) {
            ReverseSameParent(a, b);
            return;
        }

        auto a_p_2 = base_.SplitAt_2(a);
        auto a_next = a_p_2 ? *a_p_2 : a_p;

        base_.SplitAt_2(Next(b));
        auto b_next = b_p;

        base_.Reverse(a_next, b_next);
        TryMergeSides(a_p);
        TryMergeSides(base_.parent(b));
    }

    bool LineOrdered(Index a, Index b) const
    {
        return base_.LineOrdered(a, b);
    }

    std::vector<Index> Order() const {
        return base_.Order();
    }

    const TwoLevelTreeBase& base() const {
        return base_;
    }

private:
    std::experimental::optional<TwoLevelTreeBase::ParentIt> SplitOversized(TwoLevelTreeBase::ParentIt parent) {
        auto& p = *parent;
        if (p.size() <= sz_bound) return {};

        auto new_size = p.size() / 2;
        auto new_seg_begin = base_.Advance(base_.seg_begin(parent), p.size() - new_size);

        return base_.SplitAt_2(new_seg_begin);
    }

    // invalidates previously used iterators
    void TryMergeSides(TwoLevelTreeBase::ParentIt it) {
        TwoLevelTreeBase::ParentIt prev, next;
        while (it != base_.parent_begin() && (prev = std::prev(it))->size() + it->size() <= sz_bound) it = base_.MergeRight(prev);
        while ((next = std::next(it)) != base_.parent_end() && next->size() + it->size() <= sz_bound) it = base_.MergeRight(it);
    }

    void ReverseSameParent(Index a, Index b) {
        auto p = base_.parent(a);

        auto a_p = base_.SplitAt_2(a);
        base_.SplitAt_2(Next(b));

        base_.Reverse(a_p ? *a_p : p);
        TryMergeSides(p);
    }

    friend std::ostream& operator<<(std::ostream& out, const TwoLevelTreeTour& base);
};

inline std::ostream& operator<<(std::ostream& out, const TwoLevelTreeTour& tour) {
    return out << tour.base_;
}