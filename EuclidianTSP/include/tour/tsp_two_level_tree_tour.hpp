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
    Index Next(Index city) {
        auto& elem = elements[city];
        return elem.parent->reverse ? elem.prev : elem.next;
    }

    Index Prev(Index city) {
        auto& elem = elements[city];
        return elem.parent->reverse ? elem.next : elem.prev;
    }

    bool Between(Index a, Index b, Index c) {
        auto a_p = elements[a].parent;
        auto b_p = elements[b].parent;
        auto c_p = elements[c].parent;
        auto one_seg = [=]() {
            auto sp_a = elements[a].segPos;
            auto sp_b = elements[b].segPos;
            auto sp_c = elements[c].segPos;
            return (!a_p->reverse && ((sp_a < sp_b && sp_b < sp_c) ||
                                      (sp_c < sp_a && sp_a < sp_b) ||
                                      (sp_b < sp_c && sp_c < sp_a))) ||
                   (a_p->reverse && ((sp_c < sp_b && sp_b < sp_a) ||
                                     (sp_b < sp_a && sp_a < sp_c) ||
                                     (sp_a < sp_c && sp_c < sp_b)));
        };
        // could be done using a loop with shifts... as that's exactly what's going on.
        if (a_p == b_p) {
            if (c_p == b_p) return one_seg();

            return (a_p->reverse && elements[b].segPos < elements[a].segPos) ||
                   (!a_p->reverse && elements[a].segPos < elements[b].segPos);

        } else if (a_p == c_p) {
            if (b_p == c_p) return one_seg();

            return (a_p->reverse && elements[a].segPos < elements[c].segPos) ||
                   (!a_p->reverse && elements[c].segPos < elements[a].segPos);

        } else if (b_p == c_p) {
            if (a_p == c_p) return one_seg();

            return (b_p->reverse && elements[c].segPos < elements[b].segPos) ||
                   (!b_p->reverse && elements[b].segPos < elements[c].segPos);

        } else {
            // all are distinct
            // reverse bit doesn't count
            return (a_p->pos < b_p->pos && b_p->pos < c_p->pos) ||
                   (c_p->pos < a_p->pos && a_p->pos < b_p->pos) ||
                   (b_p->pos < c_p->pos && c_p->pos < a_p->pos);
        }
    }

    void Flip(Index a, Index b, Index c, Index d) {


    }

    void Reverse(Index a, Index b) {
        auto& el_a = elements[a];
        auto& el_b = elements[b];

        auto a_p = el_a.parent;
        auto b_p = el_b.parent;
        if (a_p == b_p) {
            if (elements[a].segPos > elements[b].segPos) {
                swap(a, b);
                swap(a_p, b_p);
            }
            while (a != b) {
                auto& el_a = elements[a];
                auto& el_b = elements[b];

                swap(el_a.segPos, el_b.segPos);

                elements[el_a.next].prev = b;
                elements[el_a.prev].next = b;

                elements[el_b.next].prev = a;
                elements[el_b.prev].next = a;

                swap(el_a.next, el_b.next);
                swap(el_a.prev, el_b.prev);
            }

            return;
        }

        auto ListSwap = [&](list<Parent>::iterator it_1, list<Parent>::iterator it_2) {
            auto ins_it_1 = next(it_2);
            parents.splice(it_1, parents, it_2);
            parents.splice(ins_it_1, parents, it_1);
        };

        // don't have the same parent
        if (a_p->pos > b_p->pos) {
            swap(a, b);
            ListSwap(a_p, b_p);
        }

        // be careful as it can go out of bounds
        for (auto p = std::next(a_p); p != b_p; ++p) {
            p->reverse = !p->reverse;
        }
//        same
    }

    void Reverse() {


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
};