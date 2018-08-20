#pragma once

#include "tsp_util.hpp"


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

struct TwoLevelTreeBase {
    // TODO investigate possibility of using a vector
    // will need to use list for this
    std::list<Parent> parents;
    // index is a city
    std::vector<SegElement> elements;

    using ConstParentIt = std::list<Parent>::const_iterator;
    using ParentIt = std::list<Parent>::iterator;

public:
    TwoLevelTreeBase(Count count) {
        parents.emplace_back(false, 0, 0, count-1, count);

        elements.resize(count);
        for (auto i = 0; i < count; ++i) {
            auto& el = elements[i];
            el.prev = prev_ring_index(i, count);
            el.next = next_ring_index(i, count);
            el.segPos = i;
            el.parent = parents.begin();
        }
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

    Index seg_begin(ConstParentIt it) const {
        return it->reverse ? it->segEnd : it->segBegin;
    }

    Index seg_end(ConstParentIt it) const {
        return it->reverse ? it->segBegin : it->segEnd;
    }

    Index& seg_begin(ParentIt it) {
        return it->reverse ? it->segEnd : it->segBegin;
    }

    Index& seg_end(ParentIt it) {
        return it->reverse ? it->segBegin : it->segEnd;
    }

    void Reverse(ParentIt p) {
        ReverseBounds(p, p);
        p->reverse = !p->reverse;
    }

    ParentIt parent(Index city) {
        return elements[city].parent;
    }

    void Reverse(ParentIt p_a, ParentIt p_b) {
        if (p_a == p_b) Reverse(p_a);

        ReverseBounds(p_a, p_b);

        auto last = std::next(p_b);

        std::list<Parent> tmp;
        auto pos = p_a->pos;
        tmp.splice(tmp.begin(), parents, p_a, last);
        tmp.reverse();
        for (auto& item : tmp) {
            item.pos = pos++;
            item.reverse = !item.reverse;
        }
        parents.splice(last, tmp);
    }

    template<class Func>
    void ForEach(list<Parent>::const_iterator parent, Func&& func) const {
        ForEach(seg_begin(parent), seg_end(parent), func);
    }

    template<class Func>
    void ForEach(Index a, Index b, Func&& func) const {
        for (;;) {
            func(a);
            if (a == b) return;
            a = Next(a);
        }
    }

    // does not modify state of object
    // changes representation
    void Dereverse(list<Parent>::iterator parent) {
        auto& p = *parent;

        if (!p.reverse) return;

        auto a = parent->segBegin;
        auto b = parent->segEnd;
        for (auto i = 0; i < p.sz/2; ++i) {
            auto a_next = elements[a].next;
            auto b_prev = elements[b].prev;

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

        swap(p.segBegin, p.segEnd);
        parent->reverse = false;

        int i = 0;
        i++;
    }

    std::experimental::optional<ParentIt> SplitAt_2(Index a) {
        auto parent = elements[a].parent;

        // also works if a single element
        if (seg_begin(parent) == a) {
            return {};
        }

        auto& p = *parent;

        Dereverse(parent);

        auto new_seg_begin = a;
        auto new_seg_end = p.segEnd;
        // exists by condition earlier
        p.segEnd = elements[new_seg_begin].prev;
        auto new_size = CountBetween(a, new_seg_end);
        p.sz -= new_size;

        auto new_parent = parents.emplace(std::next(parent), false, p.pos+1, new_seg_begin, new_seg_end, new_size);
        for_each(std::next(new_parent), parents.end(), [](auto& p) {
            p.pos += 2;
        });

        ForEach(new_seg_begin, new_seg_end, [&, pos=0](auto city) mutable {
            auto& el = elements[city];
            el.segPos = pos++;
            el.parent = new_parent;

        });

        return {new_parent};
    }

    void ReindexParents() {
        Index index = 0;
        for (auto& p : parents) {
            p.pos = index++;
        }
    }

    ParentIt MergeAdjacent(ParentIt left, ParentIt right) {
        if (left->reverse != right->reverse) {
            if (left->reverse) Dereverse(left);
            else Dereverse(right);
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

    std::vector<Index> Order() const {
        std::vector<Index> order;
        order.reserve(elements.size());
        auto a = seg_begin(parents.begin());
        auto b = a;
        order.push_back(b);
        while (a != (b = Next(b))) {
            order.push_back(b);
        }
        return order;
    }

private:
    void ReverseBounds(ParentIt p_a, ParentIt p_b) {
        auto a = seg_begin(p_a);
        auto b = seg_end(p_b);

        auto prev_a = Prev(a);
        auto next_b = Next(b);

        if (a != next_b) {

            Next(prev_a) = b;
            Prev(next_b) = a;

            Prev(a) = next_b;
            Next(b) = prev_a;
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const TwoLevelTreeBase& base);
};

inline std::ostream& operator<<(std::ostream& out, const TwoLevelTreeBase& tour) {
    auto& ps = tour.parents;
    for (auto it = ps.begin(); it != ps.end(); ++it) {
        out << "[";
        tour.ForEach(it, [&](auto city) {
            out << city << "(" << tour.Prev(city) << "," << tour.Next(city) << ")" << ",";
            assert(it == tour.elements[city].parent);
        });
        out << "]";
    }
    return out;
}
