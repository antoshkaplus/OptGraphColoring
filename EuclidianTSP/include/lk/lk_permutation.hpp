#pragma once

#include <range/v3/all.hpp>

#include "lk_util.hpp"


template <class Tour>
class LK_Permutation {

    Tour& tour;

    struct Item {
        City city;
        Index left;

        Item() = default;
        Item(City city) : city(city) {}
    };

    std::vector<Item> items;
    vector<Index> items_line_order;
public:
    using tour_type = Tour;

    LK_Permutation(Tour& tour) : tour(tour) {}

    template <typename... TCities>
    LK_Permutation(Tour& tour, TCities... cities) : tour(tour) {
        Assign(cities...);
    }

    City back() const {
        return items.back().city;
    }

    City front() const {
        return items.front().city;
    }

    Count size() const {
        return items.size();
    }

    City city(Index index) const {
        return items[index].city;
    }

    bool Exists(City city) {
        auto it = std::lower_bound(items_line_order.begin(), items_line_order.end(), city, [&](Index index, City city) {
            return tour.LineOrdered(items[index].city, city);
        });

        if (it == items_line_order.end()) return false;

        return items[*it].city == city;
    }

    template <typename... TCities>
    void Assign(City c_1, City c_2, TCities... cities) {
        Reset(c_1, c_2);
        Push(cities...);
    }

    template <typename... TCities>
    void Push(City c_1, City c_2, TCities... cities) {
        Push(c_1, c_2);
        Push(cities...);
    }

    void Reset(City c_1, City c_2) {
        items.resize(2);
        items[0].city = c_1;
        items[0].left = 1;

        items[1].city = c_2;
        items[1].left = 0;

        items_line_order.resize(2);
        items_line_order[0] = 0;
        items_line_order[1] = 1;
        if (!tour.LineOrdered(items[0].city, items[1].city)) {
            std::swap(items_line_order[0], items_line_order[1]);
        }
    }

    void Push(City c_1, City c_2) {

        Index index_1 = items.size();
        Index index_2 = index_1 + 1;

        items.emplace_back(c_1);
        items.emplace_back(c_2);

        auto comp = [&](Index i_1, Index i_2) {
            return tour.LineOrdered(items[i_1].city, items[i_2].city);
        };

        if (!comp(index_1, index_2)) {
            std::swap(index_1, index_2);
        }

        Index order_index_1;
        Index order_index_2;
        if (comp(index_1, items_line_order.front()) && comp(items_line_order.back(), index_2)) {
            auto it_1 = items_line_order.insert(items_line_order.begin(), index_1);
            order_index_1 = it_1 - items_line_order.begin();
            auto it_2 = items_line_order.insert(items_line_order.end(), index_2);
            order_index_2 = it_2 - items_line_order.begin();
            // swap placement is important
            std::swap(index_1, index_2);
            std::swap(order_index_1, order_index_2);
        } else {
            auto it = std::upper_bound(items_line_order.begin(), items_line_order.end(), index_1, comp);
            auto it_1 = items_line_order.insert(it, {index_1, index_2});
            order_index_1 = it_1 - items_line_order.begin();
            order_index_2 = order_index_1 + 1;
        }

        auto prev_order_index = prev_ring_index(order_index_1, items_line_order.size());
        auto prev_index = items_line_order[prev_order_index];
        items[prev_index].left = index_1;
        items[index_1].left = prev_index;

        auto next_order_index = next_ring_index(order_index_2, items_line_order.size());
        auto next_index = items_line_order[next_order_index];
        items[next_index].left = index_2;
        items[index_2].left = next_index;
    }

    void Pop() {

        auto index_1 = items.size()-1;
        auto index_2 = index_1 - 1;

        auto comp = [&](Index i_1, Index i_2) {
            return tour.LineOrdered(items[i_1].city, items[i_2].city);
        };

        if (!comp(index_1, index_2)) {
            std::swap(index_1, index_2);
        }

        if (index_1 == items_line_order.front() && items_line_order.back() == index_2) {
            items_line_order.erase(items_line_order.begin());
            items_line_order.pop_back();

            items[items_line_order.front()].left = items_line_order.back();
            items[items_line_order.back()].left = items_line_order.front();

        } else {
            auto it = std::lower_bound(items_line_order.begin(), items_line_order.end(), index_1, comp);
            auto order_index = it - items_line_order.begin();

            auto prev_order_index = prev_ring_index(order_index, items_line_order.size());
            auto next_order_index = next_ring_index(next_ring_index(order_index, items_line_order.size()), items_line_order.size());

            items[items_line_order[prev_order_index]].left = items_line_order[next_order_index];
            items[items_line_order[next_order_index]].left = items_line_order[prev_order_index];

            items_line_order.erase(it, it+2);
        }

        items.pop_back();
        items.pop_back();

    }

    // bottleneck is here unable to get to large K because of it.
    // if only could do it faster than n*log (n) or something
    template <bool kVerbose = false>
    bool CanClose() {
        if (items.size() % 2 != 0 || items.size() < 4) return false;

        return items.size() == VisitBrokenEndpoint(items[0].left, 0, 0);
    }

private:

    Count VisitBrokenEndpoint(Index broken, Index start, Count iter) {
        assert(iter < items.size());

        return 1 + VisitAddedEndpoint(added_endpoint(broken, items.size()), start, iter+1);
    }

    Count VisitAddedEndpoint(Index added, Index start, Count iter) {
        assert(iter < items.size());

        return 1 + (added == start ? 0 : VisitBrokenEndpoint(items[added].left, start, iter+1));
    }

    template<typename T>
    friend ostream& operator<<(ostream& out, const LK_Permutation<T>& perm);
};

template <typename Tour>
ostream& operator<<(ostream& out, const LK_Permutation<Tour>& perm) {
    vector<City> cities = perm.items | ranges::view::transform([](auto& item) { return item.city; });
    vector<Index> left = perm.items | ranges::view::transform([](auto& item) { return item.left; });
    Println(out, "perm:");
    Println(out, "cities: ", cities);
    Println(out, "left: ", left);
    Println(out, "line order: ", perm.items_line_order);
    return out;
}