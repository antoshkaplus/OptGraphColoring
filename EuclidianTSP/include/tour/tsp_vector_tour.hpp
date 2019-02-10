#pragma once

#include "tsp_util.hpp"


// after reverse the same order direction is not guranteed.
// it would require additional checks for prev and next.
// maybe in the future if will need to support it.
class VectorTour {
    vector<Index> orderToCity;
    vector<Index> cityToOrder;

public:
    VectorTour() {}

    VectorTour(Count city_count) {
        orderToCity.resize(city_count);
        iota(orderToCity.begin(), orderToCity.end(), 0);
        cityToOrder = orderToCity;
    }

    VectorTour(vector<Index> orderToCity) : orderToCity(std::move(orderToCity)), cityToOrder(this->orderToCity.size()) {
        for (auto i = 0; i < this->orderToCity.size(); ++i) {
            cityToOrder[this->orderToCity[i]] = i;
        }
    }

    const vector<Index>& OrderToCity() const {
        return orderToCity;
    }

    const vector<Index>& Order() const {
        return orderToCity;
    }

    const vector<Index>& CityToOrder() const {
        return cityToOrder;
    }

    Count Size() const {
        return orderToCity.size();
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
        auto order_1 = cityToOrder[city_1];
        auto order_2 = cityToOrder[city_2];
        auto order_3 = cityToOrder[city_3];

        return (order_1 < order_2 && order_2 < order_3) ||
               (order_3 < order_1 && order_1 < order_2) ||
               (order_2 < order_3 && order_3 < order_1);
    }

    void Flip(Index a, Index b, Index c, Index d) {
        if (a == Next(b)) swap(a, b);
        if (c == Next(d)) swap(c, d);

        auto [r_1, r_2] = std::make_pair(b, c);
        if (cityToOrder[r_1] > cityToOrder[r_2]) {
            tie(r_1, r_2) = std::make_pair(d, a);
        }

        Reverse(r_1, r_2);
    }

    void Reverse(Index city_1, Index city_2) {
        auto order_1 = cityToOrder[city_1];
        auto order_2 = cityToOrder[city_2];

//      Uncomment if "after" order needs to be preserved
//        bool swapped = false;
        if (order_1 > order_2) {
            --order_1;
            ++order_2;
            swap(order_1, order_2);
//            swapped = true;
        }

        for (auto i_1 = order_1, i_2 = order_2; i_1 < i_2; ++i_1, --i_2) {
            swap(orderToCity[i_1], orderToCity[i_2]);
            swap(cityToOrder[orderToCity[i_1]], cityToOrder[orderToCity[i_2]]);
        }

//        if (swapped) {
//            for (Index i_1 = 0, i_2 = orderToCity.size()-1; i_1 < i_2; ++i_1, --i_2) {
//                swap(orderToCity[i_1], orderToCity[i_2]);
//                swap(cityToOrder[orderToCity[i_1]], cityToOrder[orderToCity[i_2]]);
//            }
//        }
    }

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

    bool LineOrdered(Index c_1, Index c_2) const
    {
        return cityToOrder[c_1] < cityToOrder[c_2];
    }

    // how to optimize to 2 times faster???
};