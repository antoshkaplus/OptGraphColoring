#pragma once

#include "tsp_util.hpp"


class TwoLevelTreeTour {

    struct Parent {
        // segment should be traversed in forward or reverse direction
        bool reverse;
        // parent position, used to define order among elements
        Index pos;
        Index segBegin;
        Index segEnd;
        Count sz;


        Parent() = default;
        Parent(bool reverse, Index pos, Index segBegin, Index segEnd, Count sz)
        : reverse(reverse), pos(pos), segBegin(segBegin), segEnd(segEnd), sz(sz) {}
    };

    struct SegElement {
        list<Parent>::iterator parent;
        Index prev;
        Index next;
        // position inside the segment, used to define order among elements
        Index segPos;
    };

    // TODO investigate possibility of using a vector
    // will need to use list for this
    std::list<Parent> parents;
    // index is a city
    std::vector<SegElement> elements;

    Count sz_bound;

    using ParentIt = std::list<Parent>::iterator;

public:
    TwoLevelTreeTour(Count city_count) {
        sz_bound = static_cast<Count>(sqrt(city_count));

        parents.emplace_back(false, 0, 0, city_count-1, city_count);

        elements.resize(city_count);
        for (auto i = 0; i < city_count; ++i) {
            auto& el = elements[i];
            el.prev = prev_ring_index(i, city_count);
            el.next = next_ring_index(i, city_count);
            el.segPos = i;
            el.parent = parents.begin();
        }

        for (auto it = parents.begin(); it != parents.end(); ++it) {
            if (it->sz > sz_bound) SplitOversized(it);
        }
    }

    Count bucket_count() const {
        return parents.size();
    }

    Index Next(Index city) const {
        auto& elem = elements[city];
        return elem.parent->reverse ? elem.prev : elem.next;
    }

    Index Prev(Index city) const {
        auto& elem = elements[city];
        return elem.parent->reverse ? elem.next : elem.prev;
    }

    Index& Next(Index city) {
        auto& elem = elements[city];
        return elem.parent->reverse ? elem.prev : elem.next;
    }

    Index& Prev(Index city) {
        auto& elem = elements[city];
        return elem.parent->reverse ? elem.next : elem.prev;
    }

    Index Advance(Index city, Count count) const {
        for (auto i = 0; i < count; ++i) {
            city = Next(city);
        }
        return city;
    }

    Count CountBetween(Index a, Index b) const {
        Count count = 1;
        while (a != b) {
            ++count;
            a = Next(a);
        }
        return count;
    }

    bool Between(Index a, Index b, Index c) const {
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

        bool a_partial = true;
        bool b_partial = true;

        // can't be the same
        auto a_next = std::next(a_p);
        if (seg_begin(a_p) == a) {
            a_next = a_p;
            a_partial = false;
        }
        auto b_prev = std::prev(b_p);
        if (seg_end(b_p) == b) {
            b_prev = b_p;
            b_partial = false;
        }

        // reverse in between of two parents
        // a_p != b_p
        if (a_next->pos <= b_prev->pos) {
            Println(cout, "reverse in between");
            Reverse(a_next, std::next(b_prev));
        }

        if (a_partial && b_partial) {
            auto [a_p_1, a_p_2] = SplitAt(a);
            auto [b_p_1, b_p_2] = SplitAt(b);

            Reverse(a_p_2, std::next(a_p_2));
            Reverse(b_p_1, b_p_2);

            Move(b_p_1, a_p_2);
            Move(a_p_2, b_p_2);

            TryMergeSides(a_p_1);
            TryMergeSides(b_p_2);
            return;
        }

        if (a_partial) {
            auto [a_p_1, a_p_2] = SplitAt(a);
            Reverse(a_p_2, a_next);
            Move(a_p_2, NextParent(b_prev));

            TryMergeSides(a_p_1);
            return;
        }

        if (b_partial) {
            auto [b_p_1, b_p_2] = SplitAt(b);
            Reverse(b_p_1, b_p_2);
            Move(b_p_1, a_next);

            TryMergeSides(b_p_2);
            return;
        }
    }

    void Reverse() {
        Reverse(parents.begin(), parents.end());
    }

    std::vector<Index> Order() const {
        std::vector<Index> order;
        order.reserve(elements.size());
        auto& p = parents.front();
        auto a = p.reverse ? p.segEnd : p.segBegin;
        auto b = a;
        order.push_back(b);
        while (a != (b = Next(b))) {
            order.push_back(b);
        }
        return order;
    }

private:
    Index& seg_begin(ParentIt it) {
        return it->reverse ? it->segEnd : it->segBegin;
    }

    Index& seg_end(ParentIt it) {
        return it->reverse ? it->segBegin : it->segEnd;
    }

    template<class Func>
    void ForEach(list<Parent>::const_iterator parent, Func&& func) const {
        auto& p = *parent;

        if (p.reverse) ForEach(p.segEnd, p.segBegin, func);
        else ForEach(p.segBegin, p.segEnd, func);
    }

    template<class Func>
    void ForEach(Index a, Index b, Func&& func) const {
        for (;;) {
            func(a);
            if (a == b) return;
            a = Next(a);
        }
    }

    void Reverse(list<Parent>::iterator first, list<Parent>::iterator last) {
        std::list<Parent> tmp;
        auto pos = first->pos;
        tmp.splice(tmp.begin(), parents, first, last);
        tmp.reverse();
        for (auto& item : tmp) {
            item.pos = pos++;
            item.reverse = !item.reverse;
        }
        parents.splice(last, tmp);
    }

    // does not modify state of object
    // changes representation
    void Dereverse(list<Parent>::iterator parent) {
        auto& p = *parent;

        if (!p.reverse) return;

        ReverseSameParent(p.segBegin, p.segEnd);
        swap(p.segBegin, p.segEnd);
        p.reverse = !p.reverse;
    }

    // use SplitAt, change signature to give bool, iterator if new element created
    // make it optional even
    void ReverseSameParent(Index a, Index b) {
        assert(elements[a].parent == elements[b].parent);

        if (elements[a].segPos > elements[b].segPos) {
            swap(a, b);
        }

        auto& p = *elements[a].parent;
        if (p.segBegin == a) p.segBegin = b;
        if (p.segEnd == b) p.segEnd = a;

        while (elements[a].segPos < elements[b].segPos) {
            auto a_next = Next(a);
            auto b_prev = Prev(b);

            auto& el_a = elements[a];
            auto& el_b = elements[b];

            swap(el_a.segPos, el_b.segPos);

            elements[el_a.next].prev = b;
            elements[el_a.prev].next = b;

            elements[el_b.next].prev = a;
            elements[el_b.prev].next = a;

            swap(el_a.next, el_b.next);
            swap(el_a.prev, el_b.prev);

            a = a_next;
            b = b_prev;
        }
    }

    std::array<std::list<Parent>::iterator, 2> SplitAt(Index a) {
        std::list<Parent>::iterator parent = elements[a].parent;
        auto& p = *parent;

        Dereverse(parent);

        auto new_seg_begin = a;
        auto new_seg_end = p.segEnd;
        p.segEnd = Prev(new_seg_begin);
        auto new_size = CountBetween(a, new_seg_end);
        p.sz -= new_size;

        Println(cout, "new size", new_size, " ", a, " ", new_seg_end);

        auto new_parent = parents.emplace(std::next(parent), false, p.pos+1, new_seg_begin, new_seg_end, new_size);
        for_each(std::next(new_parent), parents.end(), [](auto& p) {
            p.pos += 2;
        });

        ForEach(new_seg_begin, new_seg_end, [&, pos=0](auto city) mutable {
            auto& el = elements[city];
            el.segPos = pos++;
            el.parent = new_parent;

            Println(cout, pos);
        });

        return {parent, new_parent};
    }

    void SplitOversized(std::list<Parent>::iterator parent) {
        auto& p = *parent;
        if (p.sz <= sz_bound) return;

        Println(cout, "SplitOversized ", p.sz);

        Dereverse(parent);

        auto new_size = p.sz / 2;
        auto new_seg_begin = Advance(p.segBegin, p.sz - new_size);
        SplitAt(new_seg_begin);
    }

    void Move(std::list<Parent>::iterator from, std::list<Parent>::iterator to) {
        Println(cout, "Move");

        // extract from
        {
            // avoid using Prev / Next Parent
             Next(Prev(seg_begin(from))) = Next(seg_end(from));
             Prev(Next(seg_end(from))) = Prev(seg_begin(from));

//            auto prev = PrevParent(from);
//            auto next = NextParent(from);
//
//            Next(seg_end(prev)) = seg_begin(next);
//            Prev(seg_begin(next)) = seg_end(prev);
        }
        // insert before to
        {
            auto prev = PrevParent(to);
            auto next = to;
            if (next == parents.end()) next = parents.begin();

            Next(seg_end(prev)) = seg_begin(from);
            Prev(seg_begin(from)) = seg_end(prev);

            Prev(seg_begin(next)) = seg_end(from);
            Next(seg_end(from)) = seg_begin(next);
        }

        parents.splice(to, parents, from);
        ReindexParents();
    }

    ParentIt PrevParent(ParentIt it) {
        return std::prev(it == parents.begin() ? parents.end() : it);
    }

    ParentIt NextParent(ParentIt it) {
        auto next = std::next(it);
        if (next == parents.end()) return parents.begin();
        return next;
    }

    void ReindexParents() {
        Index index = 0;
        for (auto& p : parents) {
            p.pos = index++;
        }
    }

    void TryMergeSides(ParentIt it) {
        ParentIt prev, next;
        while (it != parents.begin() && (prev = std::prev(it))->sz + it->sz <= sz_bound) it = MergeAdjacent(prev, it);
        while ((next = std::next(it)) != parents.end() && next->sz + it->sz <= sz_bound) it = MergeAdjacent(it, next);
    }

    ParentIt MergeAdjacent(ParentIt left, ParentIt right) {
        if (left->sz == 0) {
            parents.erase(left);
            ReindexParents();
            return right;
        }

        if (right->sz == 0) {
            parents.erase(right);
            ReindexParents();
            return left;
        }

        if (left->reverse != right->reverse) {
            Reverse(right, std::next(right));
        }

        ForEach(right, [&, pos=left->sz](auto city) mutable {
            auto& el = elements[city];
            el.segPos = pos++;
            el.parent = left;

            Println(cout, pos);
        });

        left->sz += right->sz;
        seg_end(left) = seg_end(right);

        parents.erase(right);

        ReindexParents();
        return left;
    }


    friend std::ostream& operator<<(std::ostream& out, const TwoLevelTreeTour& tour);
};

std::ostream& operator<<(std::ostream& out, const TwoLevelTreeTour& tour) {
    auto& ps = tour.parents;
    for (auto it = ps.begin(); it != ps.end(); ++it) {
        out << "[";
        tour.ForEach(it, [&](auto city) {
            out << city << ",";
        });
        out << "]";
    }
    return out;
}