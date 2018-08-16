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

    Index Advance(Index city, Count count) const {
        for (auto i = 0; i < count; ++i) {
            city = Next(city);
        }
        return city;
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

        // reverse in between of two parents
        // a_p != b_p
        if (auto a_next = std::next(a_p); a_next != b_p) {
            Reverse(a_next, std::prev(b_p));
        }

        // exactly peace I need to write

//        // now time to handle whatever is in between
//
//        auto ListSwap = [&](list<Parent>::iterator it_1, list<Parent>::iterator it_2) {
//            auto ins_it_1 = next(it_2);
//            parents.splice(it_1, parents, it_2);
//            parents.splice(ins_it_1, parents, it_1);
//        };
//
//        // don't have the same parent
//
//        ListSwap(a_p, b_p);
//
//        // be careful as it can go out of bounds
//        for (auto p = std::next(a_p); p != b_p; ++p) {
//            p->reverse = !p->reverse;
//        }
//        // same
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

    void ReverseSameParent(Index a, Index b) {
        assert(elements[a].parent == elements[b].parent);

        if (elements[a].segPos > elements[b].segPos) {
            swap(a, b);
        }

        auto& p = *elements[a].parent;
        if (p.segBegin == a) p.segBegin = b;
        if (p.segEnd == b) p.segEnd = a;

        while (a != b) {
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

    void SplitOversized(std::list<Parent>::iterator parent) {
        auto& p = *parent;
        if (p.sz <= sz_bound) return;

        Dereverse(parent);

        auto new_size = p.sz / 2;
        p.sz -= new_size;
        auto new_seg_begin = Advance(p.segBegin, p.sz);
        auto new_seg_end = p.segEnd;
        p.segEnd = Prev(new_seg_begin);
        // have to create new guy
        auto new_parent = parents.emplace(std::next(parent), false, p.pos+1, new_seg_begin, new_seg_end, new_size);
        for_each(std::next(new_parent), parents.end(), [](auto& p) {
            p.pos += 2;
        });

        // reset setPos and parent for elements
        ForEach(new_seg_begin, new_seg_end, [&, pos=0](auto city) mutable {
            auto& el = elements[city];
            el.segPos = pos++;
            el.parent = new_parent;

            Println(cout, pos);
        });
    }


    // somehow need to insert after

    // have added to new segments to the thing.
    void Split(Index a, Index b) {
        list<Parent>::iterator a_p = elements[a].parent;
//        auto a_pos = elements[a].segPos;
        Parent a_new = *a_p;
        a_p->segEnd = elements[a].prev;
        a_new.segBegin = a;
        parents.insert(std::next(a_p), a_new);
        // for a_new items have to reinitialize their parent and segPos

        list<Parent>::iterator b_p = elements[b].parent;
//        auto b_pos = elements[b].segPos;
        Parent b_new = *b_p;
        b_p->segEnd = b;
        b_new.segBegin = elements[b].next;
        parents.insert(std::next(b_p), b_new);
        // for b_new items have to reinitialize their parent and segPos

    }

    // can get parents out of a and b
    void InterParentReverse(list<Parent>::iterator a_p, list<Parent>::iterator b_p, Index a, Index b) {
        // doesn't matter if I do it now or later as it doesn't involve things much...
        std::for_each(a_p, b_p, [](auto& p) {
            p.reverse = !p.reverse;
        });
        std::reverse(a_p, b_p);

        // have to know if this next element parent reversed or not...
        // lets imagine reverse doesn't exist
        auto& el_a = elements[a];
        auto& el_b = elements[b];

        elements[el_a.prev].next = b;
        elements[el_b.next].prev = a;

        // importance of reverse, we clearly initialize a_prev but this on got reversed
        // be careful that you can override values by mistake.
        el_a.next = el_b.next;
        el_b.prev = el_a.prev;
    }

    // now we need to merge if needed and split again... back everything + give parents new positions
    void Merge(list<Parent>::iterator a_p, list<Parent>::iterator b_p) {

        if (a_p->sz < sz_bound) {
            Merge(a_p, std::next(a_p));
            // while merging

            // after successful merge try to split...
            // if split successful move to next item,
            // else move to next item anyway
            // put this item to the list
        }
        // just check one more next that should correspond to a_p->next from the stert

        // do the same procedure with b_p
    }

    void MergeNext(list<Parent>::iterator p) {
        // figure out who's less and derive to one reverse bit
        // then merge in one element, will have to again reassign items parent and segPos
        // and update fields :
//        Index segBegin;
//        Index segEnd;
//        Count sz;

    }


    // now it's all done.

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