#pragma once

#include "tsp_util.hpp"


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