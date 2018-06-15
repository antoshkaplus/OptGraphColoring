#pragma once

#include <iostream>
#include <numeric>
#include <queue>

#include "tsp.hpp"

struct TSP_NearestInsertion : TSP_InsertionSolver {
    
    struct Record {
        City inTourCity;
        City nearestCity;
        Distance distance;
        
        Record() {}
        Record(City inTourCity,
               City nearestCity,
               Distance distance) : inTourCity(inTourCity),
                                    nearestCity(nearestCity),
                                    distance(distance) {}
        
        bool operator<(const Record& r) const {
            return distance < r.distance;
        }
        
        bool operator>(const Record& r) const {
            return r < *this;
        }
    };

    vector<City> solve(const vector<Point>& points) override {
        set<City> outTourCities;
        for (size_t i = 0; i < points.size(); i++) {
            outTourCities.insert(i);
        }
        
        vector<Edge> edges;
        vector<City> startingTour = this->startingTour(points);
        for (Index i = 0; i < startingTour.size(); i++) {
            edges.push_back(Edge(startingTour[i], startingTour[(i+1)%startingTour.size()]));
            outTourCities.erase(startingTour[i]);
        }
        
        priority_queue<Record, vector<Record>, greater<Record>> pr_queue;
        for (City c_in : startingTour) {
            City c = *NearestPoint(points, outTourCities.begin(), outTourCities.end(), points[c_in]);
            pr_queue.push(Record(c_in, c, points[c_in].Distance(points[c])));
        }
        
        size_t i = startingTour.size();
        while (i < points.size()) {
            Record r = pr_queue.top();
            pr_queue.pop();
            if (outTourCities.count(r.nearestCity)) {
                typedef vector<Edge>::iterator EdgeIt; 
                
                EdgeIt es[2];
                bool b = 0;
                for (auto e_it = edges.begin(); e_it != edges.end(); e_it++) {
                    if (e_it->hasCity(r.inTourCity)) { 
                        es[b] = e_it;
                        b = !b;
                    }
                }
                EdgeIt removingEdge = min(es[0], es[1], [&](const EdgeIt& e_0, const EdgeIt& e_1) {
                    auto &ps = points;
                    return -ps[e_0->at(0)].Distance(ps[e_0->at(1)])
                           +ps[e_0->otherCity(r.inTourCity)].Distance(ps[r.nearestCity]) <
                           
                           -ps[e_1->at(0)].Distance(ps[e_1->at(1)])
                           +ps[e_1->otherCity(r.inTourCity)].Distance(ps[r.nearestCity]);
                });
                
                City c = removingEdge->otherCity(r.inTourCity);
                edges.erase(removingEdge);
                
                edges.push_back(Edge(c, r.nearestCity));
                edges.push_back(Edge(r.inTourCity, r.nearestCity));
                
                outTourCities.erase(r.nearestCity);
                
                if (outTourCities.size()) { 
                    City c_out = *NearestPoint(points, outTourCities.begin(), outTourCities.end(), points[r.nearestCity]);
                    pr_queue.push(Record(r.nearestCity, c_out, points[r.nearestCity].Distance(points[c_out])));
                }
                
                i++;
            }
            if (outTourCities.size()) { 
                City c = *NearestPoint(points, outTourCities.begin(), outTourCities.end(), points[r.inTourCity]);
                pr_queue.push(Record(r.inTourCity, c, points[r.inTourCity].Distance(points[c])));
            }
        }
        
        return edgesToTour(edges);
    }
};