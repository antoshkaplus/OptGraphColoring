//
//  TSP_FarthestInsertion.h
//  TravelingSalesman
//
//  Created by Anton Logunov on 3/31/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __TravelingSalesman__TSP_FarthestInsertion__
#define __TravelingSalesman__TSP_FarthestInsertion__

#include <iostream>
#include <queue>
#include <cassert>

#include "tsp.hpp"

struct TSP_FarthestInsertion : TSP_InsertionSolver {
    
    struct Record {
        City outTourCity;
        City nearestInTourCity;
        Distance distance;
        
        Record() {}
        Record(City outTourCity,
               City nearestInTourCity,
               Distance distance) : outTourCity(outTourCity),
        nearestInTourCity(nearestInTourCity),
        distance(distance) {}
        
        bool operator<(const Record& r) const {
            return distance < r.distance;
        }
        
        bool operator>(const Record& r) const {
            return r < *this;
        }
    };
    
    vector<City> solve(const vector<Point>& points) override {
        vector<City> startingTour = this->startingTour(points);
        vector<Edge> edges = tourToEdges(startingTour);
        
        set<City> outTourCities;
        for (City i = 0; i < points.size(); ++i) {
            outTourCities.insert(i);
        }
        for (City i : startingTour) {
            outTourCities.erase(i);
        }
        
        vector<Record> outTour;
        for (City c : outTourCities) {
            City c_in = *NearestPoint(points, startingTour.begin(), startingTour.end(), points[c]);
            outTour.push_back(Record(c, c_in, points[c].distance(points[c_in])));
        }
        
        size_t i = startingTour.size();
        while (i < points.size()) {
            auto it = max_element(outTour.begin(), outTour.end());
            Record r = *it;
            outTour.erase(it);
       
            typedef vector<Edge>::iterator EdgeIt; 
            
            EdgeIt es[2];
            bool b = 0;
            for (auto e_it = edges.begin(); e_it != edges.end(); e_it++) {
                if (e_it->hasCity(r.nearestInTourCity)) { 
                    es[b] = e_it;
                    b = !b;
                }
            }
            assert(b==0);
            EdgeIt removingEdge = min(es[0], es[1], [&](const EdgeIt& e_0, const EdgeIt& e_1) {
                auto &ps = points;
                return -ps[e_0->at(0)].distance(ps[e_0->at(1)]) 
                       +ps[e_0->otherCity(r.nearestInTourCity)].distance(ps[r.outTourCity]) <  
                
                       -ps[e_1->at(0)].distance(ps[e_1->at(1)]) 
                       +ps[e_1->otherCity(r.nearestInTourCity)].distance(ps[r.outTourCity]);
            });
            
            City c = removingEdge->otherCity(r.nearestInTourCity);
            edges.erase(removingEdge);
            
            edges.push_back(Edge(c, r.outTourCity));
            edges.push_back(Edge(r.nearestInTourCity, r.outTourCity));
            
            for (Record& out_r : outTour) {
                double d = points[r.outTourCity].distance(points[out_r.outTourCity]);
                if (out_r.distance > d) {
                    out_r.nearestInTourCity = r.outTourCity;
                    out_r.distance = d;
                }
            }
            
            i++;
        }
        
        return edgesToTour(edges);
    }

};


#endif /* defined(__TravelingSalesman__TSP_FarthestInsertion__) */
