//
//  TSP_RandomInsertion.h
//  TravelingSalesman
//
//  Created by Anton Logunov on 4/1/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __TravelingSalesman__TSP_RandomInsertion__
#define __TravelingSalesman__TSP_RandomInsertion__

#include <iostream>

#include "tsp.hpp"


namespace ant {

namespace opt {

namespace tsp {

struct TSP_RandomInsertion : TSP_InsertionSolver {
    
    struct Vector : std::vector<City> {
        Vector(const std::vector<City>& cities) : std::vector<City>(cities) {}
    
        void insertBefore(Vector::iterator position, City city) {
            insert(position, city);
        }
        
        void insertBefore(Index i, City city) {
            insertBefore(begin()+i, city);
        }
        
        void insertAfter(Vector::iterator position, City city) {
            insert(position+1, city);
        }
        
        void insertAfter(Index i, City city) {
            insertAfter(begin()+i, city);
        }
        
        bool hasCity(City c) {
            return find(begin(), end(), c) != end();
        }
    };
     
    std::vector<City> solve(const std::vector<Point>& points) override {
        Vector tour = startingTour(points);
        std::vector<City> outCities(points.size());
        iota(outCities.begin(), outCities.end(), 0);
        // delete in tour cities
        for (int i = (int)points.size()-1; i >= 0; i--) {
            if (tour.hasCity(outCities[i])) {
                std::swap(outCities[i], outCities.back());
                outCities.pop_back();
            }
        } 
                
        for (size_t i = tour.size(); i < points.size(); i++) {
            Index ind_out = random(outCities.size());
            City city_out = outCities[ind_out];
            
            Index ind_in = NearestPoint(points, tour.begin(), tour.end(), points[city_out]) - tour.begin();
            City city_in = tour[ind_in];
            
            outCities.erase(outCities.begin()+ind_out);
            
            Index ind_prev = (ind_in + tour.size() - 1)%tour.size();
            Index ind_next = (ind_in + 1)%tour.size();
            City city_prev = tour[ind_prev];
            City city_next = tour[ind_next];
            
            
            auto &ps = points;
            // is (prev city - city out - city in) more optimum chain
            if (-ps[city_prev].Distance(ps[city_in]) 
                +ps[city_prev].Distance(ps[city_out]) <  
    
                -ps[city_next].Distance(ps[city_in]) 
                +ps[city_next].Distance(ps[city_out])) {
                
                tour.insertBefore(ind_in, city_out);
            } else {
                tour.insertAfter(ind_in, city_out);
            }
        }
        return tour;
    }
};

} // end namespace tsp

} // end namespace opt

} // end namespace ant

#endif /* defined(__TravelingSalesman__TSP_RandomInsertion__) */
