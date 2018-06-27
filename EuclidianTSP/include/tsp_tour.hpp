#pragma once

// reference: Data Structures for Travelling Salesmen
// Fredman, Johnson, McGeoch, Ostheimer

#include "tsp_util.hpp"

// goes always middle
class Updater {
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
        return (a + cityCount - 1) % cityCount;
    }
    Index Nxt(Index b, Count cityCount) {
        return (b+1) % cityCount;
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

template<class Swap>
void Reverse(Index a, Index b, Count length, Swap&& swap) {
    // picks the best possible route for reverse,
    // does the reversion
}


class Tour {
public:
    virtual Index Next(Index city) = 0;
    virtual Index Prev(Index city) = 0;
    virtual bool Between(Index a, Index b, Index c) = 0;
    virtual void Flip(Index a, Index b, Index c, Index d) = 0;
    virtual void Reverse(Index a, Index b) = 0;
};

class VectorTour {
    vector<Index> orderToCity;
    vector<Index> cityToOrder;

public:
    VectorTour() {}
    VectorTour(vector<Index> orderToCity) : orderToCity(std::move(orderToCity)), cityToOrder(this->orderToCity.size()) {
        for (auto i = 0; i < this->orderToCity.size(); ++i) {
            cityToOrder[this->orderToCity[i]] = i;
        }
    }

    const vector<Index> OrderToCity() const {
        return orderToCity;
    }

    const vector<Index> CityToOrder() const {
        return cityToOrder;
    }

    Index Next(Index city) {
        auto order = cityToOrder[city];
        if (order == orderToCity.size()-1) return orderToCity[0];
        return orderToCity[order+1];
    }

    Index Prev(Index city) {
        auto order = cityToOrder[city];
        if (order == 0) return orderToCity.back();
        return orderToCity[order-1];
    }

    bool Between(Index city_1, Index city_2, Index city_3) {
        auto order_1 = orderToCity[city_1];
        auto order_2 = orderToCity[city_2];
        auto order_3 = orderToCity[city_3];

        return (order_1 < order_2 && order_2 < order_3) ||
                (order_3 < order_1 && order_1 < order_2) ||
                (order_2 < order_3 && order_3 < order_1);
    }

    void Flip(Index a, Index b, Index c, Index d) {}

    template<class UpdaterFactory>
    void Reverse(Index city_1, Index city_2, UpdaterFactory& factory) {
        assert(city_1 < orderToCity.size());
        assert(city_2 < orderToCity.size());

        auto order_1 = cityToOrder[city_1];
        auto order_2 = cityToOrder[city_2];

        assert(order_1 < orderToCity.size());
        assert(order_2 < orderToCity.size());

        if (order_1 > order_2) {
            swap(order_1, order_2);
        }

        auto updater = factory.Create(order_1, order_2);

        // rethink this one too
        while (!updater.IsFinished()) {

            tie(order_1, order_2) = updater.values();
            assert(order_1 < orderToCity.size());
            assert(order_2 < orderToCity.size());

            auto city_1 = orderToCity[order_1];
            auto city_2 = orderToCity[order_2];

            assert(city_1 < orderToCity.size());
            assert(city_2 < orderToCity.size());

            swap(orderToCity[order_1], orderToCity[order_2]);
            swap(cityToOrder[city_1], cityToOrder[city_2]);

            updater.Next();
        }
    }

    // how to optimize to 2 times faster???
};


class TwoLevelTreeTour {
public:

    struct Parent {
        bool reverse;
        Index pos;
        Index segBegin;
        Index segEnd;
        Count sz;
    };

    struct SegElement {
        list<Parent>::iterator parent;
        Index prev;
        Index next;
        Index segPos;
    };

    // will need to use list for this

    list<Parent> parents;
    vector<SegElement> elements;

    Count sz_bound;


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
            return (a_p->pos < b_p->pos && b_p->pos < c_p->pos) || (c_p->pos < a_p->pos && a_p->pos < b_p->pos) ||
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