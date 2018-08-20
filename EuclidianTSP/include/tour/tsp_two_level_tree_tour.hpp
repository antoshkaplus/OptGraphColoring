#pragma once

#include "tsp_two_level_tree_base.hpp"


class TwoLevelTreeTour {

    TwoLevelTreeBase base;
    Count sz_bound;

public:
    TwoLevelTreeTour(Count city_count) : base(city_count) {
        // TODO solve this issue with checking if a == b 
        sz_bound = max(2, static_cast<Count>(sqrt(city_count)));

        for (auto it = base.parents.begin(); it != base.parents.end(); ++it) {
            if (it->sz > sz_bound) SplitOversized(it);
        }
    }

    Count bucket_count() const {
        return base.parents.size();
    }

    Index Next(Index city) const {
        return base.Next(city);
    }

    Index Prev(Index city) const {
        return base.Prev(city);
    }

    bool Between(Index a, Index b, Index c) const {
        auto& elements = base.elements;

        auto a_p = elements[a].parent;
        auto b_p = elements[b].parent;
        auto c_p = elements[c].parent;

        if (a_p == b_p && b_p == c_p) {

            auto sp_a = elements[a].segPos;
            auto sp_b = elements[b].segPos;
            auto sp_c = elements[c].segPos;

            return (!a_p->reverse && ((sp_a < sp_b && sp_b < sp_c) ||
                                      (sp_c < sp_a && sp_a < sp_b) ||
                                      (sp_b < sp_c && sp_c < sp_a))) ||
                   (a_p->reverse && ((sp_c < sp_b && sp_b < sp_a) ||
                                     (sp_b < sp_a && sp_a < sp_c) ||
                                     (sp_a < sp_c && sp_c < sp_b)));
        }

        if (a_p == b_p) {
            return (a_p->reverse && elements[b].segPos < elements[a].segPos) ||
                   (!a_p->reverse && elements[a].segPos < elements[b].segPos);

        }

        if (a_p == c_p) {
            return (c_p->reverse && elements[a].segPos < elements[c].segPos) ||
                   (!c_p->reverse && elements[c].segPos < elements[a].segPos);

        }

        if (b_p == c_p) {
            return (b_p->reverse && elements[c].segPos < elements[b].segPos) ||
                   (!b_p->reverse && elements[b].segPos < elements[c].segPos);
        }

        // all are distinct
        // reverse bit doesn't count
        return (a_p->pos < b_p->pos && b_p->pos < c_p->pos) ||
               (c_p->pos < a_p->pos && a_p->pos < b_p->pos) ||
               (b_p->pos < c_p->pos && c_p->pos < a_p->pos);
    }

    void Flip(Index a, Index b, Index c, Index d) {


    }

    void Reverse(Index a, Index b) {
        auto& elements = base.elements;

        auto a_p = elements[a].parent;
        auto b_p = elements[b].parent;

        if (a_p == b_p) {
            ReverseSameParent(a, b);
            return;
        }

        if (a_p->pos > b_p->pos) {
            swap(a, b);
            swap(a_p, b_p);
        }

        auto a_p_2 = base.SplitAt_2(a);
        auto a_next = a_p_2 ? a_p_2.value() : a_p;

        base.SplitAt_2(Next(b));
        auto b_next = b_p;

        base.Reverse(a_next, b_next);
        TryMergeSides(a_p);
        TryMergeSides(elements[b].parent);

//
//        // reverse in between of two parents
//        // a_p != b_p
//        if (a_next->pos <= b_prev->pos) {
//            Println(cout, "reverse in between");
//            Reverse(a_next, std::next(b_prev));
//        }
//
//
//
//
//        if (a_p_2 && b_p_2) {
//
//            Reverse(a_p_2, std::next(a_p_2));
//            Reverse(b_p_1, b_p_2);
//
//            Move(b_p_1, a_p_2);
//            Move(a_p_2, b_p_2);
//
//            TryMergeSides(a_p_1);
//            TryMergeSides(b_p_2);
//            return;
//        }
//
//        if (a_p_2) {
//            Reverse(a_p_2, a_next);
//            Move(a_p_2, NextParent(b_prev));
//
//            TryMergeSides(a_p_1);
//            return;
//        }
//
//        if (b_p_2) {
//            Reverse(b_p_1, b_p_2);
//            Move(b_p_1, a_next);
//
//            TryMergeSides(b_p_2);
//            return;
//        }
    }

    void Reverse() {
        auto& parents = base.parents;

        base.Reverse(parents.begin(), std::prev(parents.end()));
    }

    std::vector<Index> Order() const {
        return base.Order();
    }

private:

    void SplitOversized(std::list<Parent>::iterator parent) {
        auto& p = *parent;
        if (p.sz <= sz_bound) return;

        Println(cout, "SplitOversized ", p.sz);

        base.Dereverse(parent);

        auto new_size = p.sz / 2;
        auto new_seg_begin = base.Advance(p.segBegin, p.sz - new_size);

        base.SplitAt_2(new_seg_begin);
    }

    // invalidates previously used iterators
    void TryMergeSides(std::list<Parent>::iterator it) {
        if (it == base.parents.end()) return;

        std::list<Parent>::iterator prev, next;
        while (it != base.parents.begin() && (prev = std::prev(it))->sz + it->sz <= sz_bound) it = base.MergeAdjacent(prev, it);
        while ((next = std::next(it)) != base.parents.end() && next->sz + it->sz <= sz_bound) it = base.MergeAdjacent(it, next);
    }


    void ReverseSameParent(Index a, Index b) {
        auto& elements = base.elements;

        assert(elements[a].parent == elements[b].parent);

        std::list<Parent>::iterator p = elements[a].parent;
        if ((!p->reverse && elements[a].segPos > elements[b].segPos) ||
            (p->reverse && elements[a].segPos < elements[b].segPos)) {
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