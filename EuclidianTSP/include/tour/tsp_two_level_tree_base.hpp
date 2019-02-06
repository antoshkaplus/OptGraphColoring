#pragma once

#include "ant/core/sequential_indexer.hpp"

#include "tsp_util.hpp"


class TwoLevelTreeBase {

    class Parent {
        // segment should be traversed in forward or reverse direction
        bool reverse;
        // parent position, used to define order among elements
        Index pos;
        Index segBegin;
        Index segEnd;
        Count sz;

        Parent() = default;
        Parent(bool reverse, Index pos, Index segBegin, Index segEnd, Count sz)
                : reverse(reverse), pos(pos), segBegin(segBegin), segEnd(segEnd), sz(sz) {

            if (reverse) swap(this->segBegin, this->segEnd);
        }

    public:
        Count size() const {
            return sz;
        }

        Index position() const {
            return pos;
        }

        friend class TwoLevelTreeBase;
    };

    struct SegElement {
        list<Parent>::iterator parent;
        Index prev;
        Index next;
        // position inside the segment, used to define order among elements
        // doesn't have have to start from 0
        Index segPos;
    };

    std::list<Parent> parents;
    SequentialIndexer<std::list<Parent>, Index&(*)(std::list<Parent>::iterator)> indexer;

    // index is a city
    std::vector<SegElement> elements;


public:
    using ParentIt = std::list<Parent>::iterator;
    using ConstParentIt = std::list<Parent>::const_iterator;


    TwoLevelTreeBase(const std::vector<Index>& cities) : indexer(parents, [](ParentIt it) -> Index& { return it->pos; }) {

        Count count = cities.size();
        parents.emplace_back(Parent{false, 0, 0, count-1, count});

        elements.resize(count);
        for (auto i = 0; i < count; ++i) {
            auto& el = elements[cities[i]];
            el.prev = cities[prev_ring_index(i, count)];
            el.next = cities[next_ring_index(i, count)];
            el.segPos = i;
            el.parent = parents.begin();
        }
    }

    TwoLevelTreeBase(Count count) : indexer(parents, [](ParentIt it) -> Index& { return it->pos; }) {

        parents.emplace_back(Parent{false, 0, 0, count-1, count});

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

    bool seg_ordered(Index a, Index b) const {
        auto a_p = elements[a].parent;
        auto b_p = elements[b].parent;

        if (a_p != b_p) return a_p->pos < b_p->pos;

        return (!a_p->reverse && elements[a].segPos <= elements[b].segPos) ||
               (a_p->reverse && elements[a].segPos >= elements[b].segPos);
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

    void Reverse(ParentIt p) {
        Reverse(p, p);
    }

    ParentIt parent(Index city) {
        return elements[city].parent;
    }

    ConstParentIt parent(Index city) const {
        return elements[city].parent;
    }

    Count parent_count() const {
        return parents.size();
    }

    ParentIt parent_begin() {
        return parents.begin();
    }

    ParentIt parent_end() {
        return parents.end();
    }

    void Reverse(ParentIt p_a, ParentIt p_b) {

        ReverseBounds(p_a, p_b);

        auto last = std::next(p_b);

        std::list<Parent> tmp;
        tmp.splice(tmp.begin(), parents, p_a, last);
        tmp.reverse();

        auto next = tmp.begin();
        auto prev = std::prev(tmp.end());

        for (auto i = 0; i < tmp.size()/2; ++i) {
            std::swap(next->pos, prev->pos);
            next = std::next(next);
            prev = std::prev(prev);
        }
        for (auto& item : tmp) {
            item.reverse = !item.reverse;
        }

        parents.splice(last, tmp);
    }

    bool LineOrdered(Index a, Index b) const
    {
        auto a_p = elements[a].parent;
        auto b_p = elements[b].parent;

        if (a_p == b_p) {

            auto sp_a = elements[a].segPos;
            auto sp_b = elements[b].segPos;

            return (!a_p->reverse && sp_a < sp_b) ||
                   ( a_p->reverse && sp_b < sp_a);
        }

        return a_p->pos < b_p->pos;
    }

    template<class Func>
    void ForEachParent(Func&& func) const {
        for (auto it = parents.begin(); it != parents.end(); ++it) {
            func(it);
        }
    }

    template<class Func>
    void ForEachParent(Func&& func) {
        for (auto it = parents.begin(); it != parents.end(); ++it) {
            func(it);
        }
    }

    template<class Func>
    void ForEach(Index a, Index b, Func&& func) const {
        for (;;) {
            func(a);
            if (a == b) return;
            a = Next(a);
        }
    }

    void FlipReverse(ParentIt parent) {
        auto& p = *parent;

        auto a = p.segEnd;

        for (auto new_pos = 0; new_pos < p.sz; ++new_pos) {
            auto& el = elements[a];
            auto a_prev = el.prev;

            swap(el.next, el.prev);
            el.segPos = new_pos;

            a = a_prev;
        }

        swap(p.segBegin, p.segEnd);
        p.reverse = !p.reverse;
    }

    std::experimental::optional<ParentIt> SplitAt_2(Index a) {
        auto parent = elements[a].parent;

        // also works if a single element
        if (seg_begin(parent) == a) {
            return {};
        }

        auto new_seg_begin = a;
        auto new_seg_end = seg_end(parent);
        // exists by condition earlier
        seg_end(parent) = Prev(a);

        auto new_size = BetweenCountSameParent(new_seg_begin, new_seg_end);
        parent->sz -= new_size;

        auto new_parent = parents.emplace(std::next(parent), Parent{parent->reverse, 0, new_seg_begin, new_seg_end, new_size});
        indexer.Reset(new_parent);

        ForEachBeginEnd(new_parent, [&](auto city) {
            elements[city].parent = new_parent;
        });

        return {new_parent};
    }

    ParentIt MergeRight(ParentIt left) {
        auto right = std::next(left);
        if (right == parents.end()) return left;

        if (left->reverse != right->reverse) {
            FlipReverse(left->sz < right->sz ? left : right);
        }

        left->sz += right->sz;
        seg_end(left) = seg_end(right);

        ResetParent(left);

        parents.erase(right);

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

    void ResetParent(ParentIt p) {
        ForEachBeginEnd(p, [&, segPos=0](auto city) mutable {
            auto& el = elements[city];
            el.segPos = segPos++;
            el.parent = p;
        });
    }

    template<class Func>
    void ForEachBeginEnd(ConstParentIt parent, Func&& func) {
        auto begin = parent->segBegin;
        auto end = parent->segEnd;

        for (;;) {
            func(begin);
            if (begin == end) return;
            begin = elements[begin].next;
        }
    }

    Count BetweenCountSameParent(Index a, Index b) {
        return std::abs(elements[a].segPos - elements[b].segPos) + 1;
    }

    friend std::ostream& operator<<(std::ostream& out, const TwoLevelTreeBase& base);
};

inline std::ostream& operator<<(std::ostream& out, const TwoLevelTreeBase& base) {
    auto& ps = base.parents;
    for (auto it = ps.begin(); it != ps.end(); ++it) {
        out << "[";
        base.ForEach(base.seg_begin(it), base.seg_end(it), [&](auto city) {
            out << city << "(" << base.Prev(city) << "," << base.Next(city) << ")" << ",";
        });
        out << "]";
    }
    return out;
}
