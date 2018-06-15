#pragma once

#include <iostream>
#include <chrono>
#include <random>


#include "tsp.hpp"

struct TSP_NearestNeighbor : TSP {
    vector<City> solve(const vector<Point>& points) override {
        vector<City> cities(points.size());
        for (City c = 0; c < points.size(); c++) cities[c] = c;
        swap(cities[0],cities[random(points.size())]);
        City i = 0;
        // last city has no nearest point
        while (++i < points.size()-1) {
            swap(cities[i], 
                 cities[NearestPoint(
                            points, 
                            vector<City>(cities.begin()+i, cities.end()), 
                            points[cities[i-1]])]);
        }
        return cities;
    }
};
