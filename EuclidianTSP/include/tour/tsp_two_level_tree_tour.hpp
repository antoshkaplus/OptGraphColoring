#pragma once

#include "tsp_two_level_tree_base.hpp"


class TwoLevelTreeTour {

    TwoLevelTreeBase base;
    Count sz_bound;

public:
    TwoLevelTreeTour(Count city_count/*, Count sz_bound*/) : base(city_count) {
        // TODO solve this issue with checking if a == b 
        sz_bound = max(1, static_cast<Count>(sqrt(city_count)));

        base.ForEachParent([&](auto it) {
            while (SplitOversized(it));
        });
    }

    Index Next(Index city) const {
        return base.Next(city);
    }

    Index Prev(Index city) const {
        return base.Prev(city);
    }

    bool Between(Index a, Index b, Index c) const {
        return base.Between(a, b, c);
    }

    void Flip(Index a, Index b, Index c, Index d) {


    }

    void Reverse(Index a, Index b) {
        auto a_p = base.parent(a);
        auto b_p = base.parent(b);

        if (a_p == b_p) {
            ReverseSameParent(a, b);
            return;
        }

        if (a_p->position() > b_p->position()) {
            swap(a, b);
            swap(a_p, b_p);
        }

        auto a_p_2 = base.SplitAt_2(a);
        auto a_next = a_p_2 ? a_p_2.value() : a_p;

        base.SplitAt_2(Next(b));
        auto b_next = b_p;

        base.Reverse(a_next, b_next);
        TryMergeSides(a_p);
        TryMergeSides(base.parent(b));
    }

    std::vector<Index> Order() const {
        return base.Order();
    }

private:
    std::experimental::optional<TwoLevelTreeBase::ParentIt> SplitOversized(std::list<Parent>::iterator parent) {
        auto& p = *parent;
        if (p.size() <= sz_bound) return {};

        auto new_size = p.size() / 2;
        auto new_seg_begin = base.Advance(base.seg_begin(parent), p.size() - new_size);

        return base.SplitAt_2(new_seg_begin);
    }

    // invalidates previously used iterators
    void TryMergeSides(TwoLevelTreeBase::ParentIt it) {
        TwoLevelTreeBase::ParentIt prev, next;
        while (it != base.parent_begin() && (prev = std::prev(it))->size() + it->size() <= sz_bound) it = base.MergeRight(prev);
        while ((next = std::next(it)) != base.parent_end() && next->size() + it->size() <= sz_bound) it = base.MergeRight(it);
    }


    void ReverseSameParent(Index a, Index b) {
        auto p = base.parent(a);
        if (!base.in_seg_order(a, b)) {
            swap(a, b);
        }

        auto a_p = base.SplitAt_2(a);
        base.SplitAt_2(Next(b));

        base.Reverse(a_p ? a_p.value() : p);
        TryMergeSides(p);
    }

    friend std::ostream& operator<<(std::ostream& out, const TwoLevelTreeTour& base);
};

inline std::ostream& operator<<(std::ostream& out, const TwoLevelTreeTour& tour) {
    return out << tour.base;
}